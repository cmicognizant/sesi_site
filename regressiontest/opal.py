#!/usr/bin/env python
from __future__ import unicode_literals

import os
import six
from six.moves.urllib.parse import urlparse
import time
from selenium.common.exceptions import TimeoutException, NoSuchElementException
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.by import By

from pylenium import Pylenium
import pylenium.conditions as C

from browser import Page
from helpers import *

class OpalPage (Page):
    @property
    def location(self):
        return urlparse(self.driver.current_url).fragment.lstrip('!')

    def onPage(self):
        return self.loggedIn() and type(self).url_location == self.location

    def waitFor(self, **kwargs):
        self.waitForCondition(self.onPageContents, **kwargs)
        return self

    def loggedIn(self):
        if LoginPage(self.driver).onPage():
            return False
        if self.location == 'dashboard':
            self.driver.find_element(link_text='Projects').click()
        else:
            self.driver.find_element(link_text='Dashboard').click()
        self.driver.back()
        time.sleep(1)
        return not LoginPage(self.driver).onPage()

    def ensureLoggedIn(self, username, password):
        if self.loggedIn():
            return
        loginpage = LoginPage(self.driver)
        loginpage.waitFor()
        newpage = loginpage.logIn(username, password)
        assert self.loggedIn()
        return newpage

    def findCurrentPage(self):
        loc = self.location
        for cls in OpalPage.__subclasses__():
            try:
                if cls.url_location == loc: return cls(self.driver)
            except AttributeError:
                continue
        raise TestException('unable to identify current page: ' + self.driver.url)

    def ensureScreenWidth(self):
        size = self.driver.window_size
        if size[0] < 1050:
            self.driver.window_size = (1050, size[1])
        assert self.driver.window_size[0] >= 1050

    def logOut(self):
        if self.loggedIn():
            self.driver.find_element(link_text=self.browser.username).click()
            self.driver.find_element(link_text='Logout').click()
        return LoginPage(self.driver).waitFor()

    def findByLabel(self, label, class_='gwt-TextBox', label_element='label', target_element='input'):
        return self.findElement(xpath=find_by_label(label, 
                                                    class_=class_, 
                                                    label_element=label_element, 
                                                    target_element=target_element))

    def gotoDashboard(self):
        self.findElement(link_text='Dashboard').click()
        return Dashboard(self.driver).waitFor()

    def gotoProjects(self):
        self.findElement(link_text='Projects').click()
        return Projects(self.driver).waitFor()


class LoginPage (OpalPage):
    def onPage(self):
        return self.onPageContents()

    def onPageContents(self):
        return self.elementExists(tag='label', contains_text='User Name') and \
            self.elementExists(tag='label', contains_text='Password') and \
            self.elementExists(tag='a', contains_text='Sign In')

    def logIn(self, username, password):
        username_field = self.findByLabel('User Name')
        password_field = self.findByLabel('Password', 'gwt-PasswordTextBox')
        username_field.send_keys(username)
        password_field.send_keys(password + '\n')
        self.driver.wait_until(C.element_present(link_text='Dashboard'))
        return self.findCurrentPage()


class Dashboard (OpalPage):
    url_location = 'dashboard'

    def onPageContents(self):
        return self.elementExists(tag='h1', contains_text='Dashboard')

    def gotoExploreData(self):
        return self.gotoProjects()
        

class Projects (OpalPage):
    url_location = 'projects'

    def onPageContents(self):
        return self.elementExists(tag='h1', contains_text='Projects') and self._projectlinks()

    def _projectlinks(self, contains=''):
        return self.driver.find_elements(css="tr div a[href^='#!project;name=']", contains_text=contains)

    def gotoProject(self, name):
        projects = self._projectlinks(name)
        assert projects
        projects[0].click()
        return Project(self.driver, name).waitFor()

    def createProject(self, name):
        assert not self.hasProject(name)
        self.driver.find_element(link_text='Add Project').click()
        self.waitForCondition(lambda: self.findByLabel('Name'))
        self.findByLabel('Name').send_keys(name)
        self.findByLabel('Title').send_keys(name)
        self.driver.find_element(link_text='Save').click()
        self.waitForCondition(C.not_or_gone(C.element_present(xpath=find_by_label('Name'))))
        self.waitForCondition(lambda: self.hasProject(name))

    def hasProject(self, name):
        return bool(self._projectlinks(name))


class Project (OpalPage):
    def __init__(self, driver, name):
        super(Project, self).__init__(driver)
        self.name = name
        self.selector = "tr div a[href^='#!project;name=%s;tab=TABLES;path=']" % css_escape(name)

    def onPageContents(self):
        # The 'No tables' message is visible before the tables are loaded
        time.sleep(.5)
        return self.elementExists(xpath=contains('span', 'No tables', class_='gwt-InlineLabel')) or \
            self.elementExists(css=self.selector)

    def removeProject(self):
        self.findElement(css='li[title=Administration] a').click()
        self.findElement(link_text='Remove Project').click()
        self.waitForCondition(C.element_present(xpath=contains('h3', 'Remove Project')))
        self.findElement(link_text='Yes').click()
        return Projects(self.driver).waitFor()

    def tables(self):
        # Link text is table name prepended with space
        return [e.text[1:] for e in self.driver.find_elements(css=self.selector)]

    def hasTable(self, name):
        return any(e == name for e in self.tables())

    def removeTable(self, name):
        self.findElement(xpath=contains('td', name)+'/preceding-sibling::td//input').click()
        self.findElement(link_text='Remove').click()
        # confirm removal
        self.findElement(link_text='Yes').click()
        self.waitFor()

    def removeAllTables(self):
        self.findElement(xpath=contains('th', 'Name')+'/preceding-sibling::th//input').click()
        if not self.elementExists(link_text='Remove'):
            # If no tables exist, the remove button does not appear
            return
        self.findElement(link_text='Remove').click()
        # confirm removal
        self.findElement(link_text='Yes').click()
        self.waitFor()

    def addTable(self, name):
        self.findElement(link_text='Add Table').click()
        self.waitForCondition(lambda: self.elementExists(link_text='Add table...'))
        self.findElement(link_text='Add table...').click()
        self.waitForCondition(lambda: self.findByLabel('Name'))
        self.findByLabel('Name').send_keys(name)
        self.findElement(link_text='Save').click()
        return Table(self.driver, self.name, name).waitFor()

    def gotoTable(self, name):
        for e in self.driver.find_elements(css=self.selector):
            if e.text == ' '+name:
                e.click()
                break
        return Table.detect(self.driver, self.name, name)

    def uploadAndSelect(self, filepath):
        assert os.path.isabs(filepath)
        assert os.path.isfile(filepath)
        filename = os.path.basename(filepath)

        # upload file from local system
        self.findElement(link_text='Upload').click()
        # upload dialog
        self.findElement(name='fileToUpload').send_keys(filepath)
        self.findElement(link_text='Upload').click()
        self.waitForCondition(C.any(C.element_present(xpath=contains('h3', 'Replace File')),
                                    C.element_present(xpath=contains('h3', 'File Selector'))))
        # replace file dialog
        if self.elementExists(xpath=contains('h3', 'Replace File')):
            self.findElement(link_text='Yes').click()
        # file selection dialog
        selector = contains('td', filename)+'/preceding-sibling::td//input'
        time.sleep(2)
        self.waitForCondition(C.element_visible(xpath=selector))
        self.findElement(xpath=selector).click()
        self.findElement(link_text='Select').click()

    def importTable(self, filepath, tablename):
        self.findElement(link_text='Import').click()
        self.waitForCondition(lambda: self.elementExists(css='div.gwt-Label', contains_text='Data Format'))

        # Import dialog - select data format
        # The default selected format in the dialog is CSV. If this fails, implement better code to find the select box.
        self.findElement(link_text='CSV').click()
        finder = contains('li', 'Opal Archive')
        self.waitForCondition(lambda: self.elementExists(xpath=finder))
        self.findElement(xpath=finder).click()
        self.findElement(link_text='Next >').click()

        # select data file
        self.findElement(link_text='Browse').click()
        self.uploadAndSelect(filepath)

        # back in import dialog
        self.findElement(link_text='Next >').click()
        self.findElement(link_text='Next >').click()
        # select our table
        finder = (contains('td', tablename, 
                          prefix='//div[%s]//' % xpath_class('modal'))
                  + '/preceding-sibling::td//input')
        self.waitForCondition(C.element_present(xpath=finder))
        self.findElement(xpath=finder).click()
        self.findElement(link_text='Next >').click()
        self.findElement(link_text='Next >').click()
        self.findElement(link_text='Finish').click()
        
        # Back at the main screen, wait for the import job to finish
        while not self.hasTable(tablename):
            self.driver.refresh()
            self.waitFor()
            time.sleep(1)

    def importView(self, name, tables, filepath):
        #import pdb; pdb.set_trace()
        self.findElement(link_text='Add Table').click()
        self.findElement(link_text='Add view...').click()

        self.findByLabel('Name').send_keys(name)
        for table in tables:
            # The Table References field is a javascript custom dropdown. The headers with the projects
            # only appear once you start typing into the field.
            references_field = self.findByLabel('Table References', class_=None)
            references_field.click()
            references_field.clear()
            time.sleep(.5)
            references_field.send_keys(table)
            time.sleep(.5)
            xpath = contains('li', self.name, prefix=find_by_label('Table References', 
                                                                   class_=None, target_element='li')
                             +'/self::')
            project_lis = self.waitForCondition(lambda: self.driver.find_elements(xpath=xpath))
            assert len(project_lis) == 1, ("Project not found or name collision between project "
                                           "%s and another table or project name" % self.name)
            project_li = project_lis[0]
            target = project_li.find_element(xpath=contains('following-sibling::li', table, prefix=''))
            self.waitForCondition(C.element_visible(element=target))
            target.click()
        self.findElement(link_text='Browse').click()
        self.uploadAndSelect(filepath)
        self.findElement(link_text='Save').click()
        return Table.detect(self.driver, self.name, name)


class Table (OpalPage):
    @classmethod
    def detect(cls, driver, *args):
        subclasses = [Table, TableSummary, TablePermissions]
        driver.wait_until(C.any(*((lambda _, T=T: T(driver, *args).onPageContents()) for T in subclasses)))
        for T in subclasses:
            p = T(driver, *args)
            if p.onPageContents():
                if T is Table:
                    return p
                return p.gotoDictionary()

    def __init__(self, driver, projectname, tablename):
        super(Table, self).__init__(driver)
        self.projectname = projectname
        self.tablename = tablename

    def onPageContents(self):
        return (self.elementExists(link_text='Add Variable') or 
                self.elementExists(link_text='Add Variables')) \
            and self.elementExists(css='h3', contains_text='Tables')

    def gotoProject(self):
        self.findElement(link_text=self.projectname).click()
        return Project(self.driver, self.projectname).waitFor()

    def gotoSummary(self):
        self.findElement(link_text='Summary').click()
        return TableSummary(self.driver, self.projectname, self.tablename).waitFor()

    def gotoDictionary(self):
        self.findElement(link_text='Dictionary').click()
        return Table(self.driver, self.projectname, self.tablename).waitFor()

    def gotoPermissions(self):
        self.findElement(link_text='Permissions').click()
        return TablePermissions(self.driver, self.projectname, self.tablename).waitFor()        

class TableSummary (Table):
    def onPageContents(self):
        return self.elementExists(xpath=contains('div', 'Table values index', class_='gwt-Label'))

    def index(self):
        self.waitForCondition(C.element_present(link_text='Index Now'),
                              C.element_present(link_text='Remove Index'),
                              C.element_present(link_text='Cancel'))
        if self.elementExists(link_text='Index Now'):
            self.findElement(link_text='Index Now').click()
        self.waitForCondition(C.element_present(link_text='Remove Index'), timeout=60)

class TablePermissions (Table):
    def onPageContents(self):
        return self.elementExists(link_text='Add Permission')

    def addUserPermission(self, user, level):
        if level != 'View dictionary and summaries':
            raise NotImplementedError("Only the default level of 'View dictionary and summaries' is implemented")
        self.findElement(link_text='Add Permission').click()
        self.findElement(link_text='Add user permission...').click()
        self.findByLabel('Name').send_keys(user)
        self.findElement(link_text='Save').click()
        self.waitForCondition(C.element_present(xpath=contains('td', user)))

    def getPermission(self, name):
        tablerow = self.driver.find_elements(xpath=contains('td', name)+'/following-sibling::td')
        if not tablerow:
            return None
        return tablerow[1].text
        


def not_find_by_label(driver, label, class_='gwt-TextBox'):
    try:
        return not driver.find_element(xpath=find_by_label(label, class_))
    except NoSuchElementException:
        return True

def find_by_label(label, class_='gwt-TextBox', label_element='label', target_element='input'):
    """XPath expression to find an <input> element with a certain class that is preceded by a <label> 
    containing the given text"""
    return (contains(label_element, label) +
            "/following-sibling::*/descendant-or-self::%s[%s]" % (
                target_element,
                xpath_class(class_)))
