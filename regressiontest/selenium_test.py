#!/usr/bin/env python
from __future__ import unicode_literals, division, absolute_import, print_function

import os
import atexit
from pprint import pprint
import yaml
from yaml.nodes import MappingNode, ScalarNode
from yaml.constructor import ConstructorError
import argparse

from selenium import webdriver
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.by import By

import pylenium
import pylenium.conditions as C

argparser = argparse.ArgumentParser(description='Test Opal and Mica')
argparser.add_argument('--data_dir', default='', help=
                       'The directory where selenium_test can find needed data files and python libraries')
argparser.add_argument('--config', '-c', type=argparse.FileType('r'), help=
                       'The user configuration file, defaults to config.yaml')
argparser.add_argument('--delete', '-D', action='store_true', help=
                       'If set, delete test data after testing')
argparser.add_argument('--only-opal', action='store_true', help=
                       'Only run the Opal tests')
argparser.add_argument('--only-mica', action='store_true', help=
                       'Only run the Mica test, assume the Opal data is set up and ready to go')
args = argparser.parse_args()
if args.data_dir:
    if not os.path.isdir(args.data_dir):
        args.error('data_dir is not a directory')
    os.chdir(args.data_dir)


# We need to process the --data_dir flag before importing these
from browser import Browser
from mica import *
from opal import *
from helpers import namespace


if not args.config:
    try:
        if not os.path.isfile('config.yaml'):
            args.error("Configuration file not found! "+os.path.join(args.data_dir, 'config.yaml'))
        args.config = open('config.yaml')
    except OSError as e:
        args.error("Unable to open configuration file: "+str(e))

if args.only_opal:
    args.run_opal = True
    args.run_mica = False
elif args.only_mica:
    args.run_opal = False
    args.run_mica = True
else:
    args.run_opal = args.run_mica = True


# Configure yaml to use our modified dict class so we have attribute access
# and preserve order.
# Note: this will affect yaml process-wide, but for our case that is ok.

# Based on yaml.constructor.SafeConstructor.construct_yaml_map 
# and yaml.loader.BaseConstructor.construct_mapping
def construct_map(loader, node):
    d = namespace()
    yield d
    assert isinstance(node, MappingNode)
    for key_node, value_node in node.value:
        key = loader.construct_object(key_node)
        try:
            hash(key)
        except TypeError as exc:
            raise ConstructorError("while constructing a mapping", node.start_mark,
                    "found unacceptable key (%s)" % exc, key_node.start_mark)
        value = loader.construct_object(value_node)
        d[key] = value
yaml.add_constructor('tag:yaml.org,2002:map', construct_map)

# for debugging purposes
# based on yaml.representer.Representer.represent_mapping
def represent_map(self, mapping):
    value = []
    node = MappingNode('tag:yaml.org,2002:map', value)
    if self.alias_key is not None:
        self.represented_objects[self.alias_key] = node
    best_style = True
    if hasattr(mapping, 'items'):
        mapping = mapping.items()
    for item_key, item_value in mapping:
        node_key = self.represent_data(item_key)
        node_value = self.represent_data(item_value)
        if not (isinstance(node_key, ScalarNode) and not node_key.style):
            best_style = False
        if not (isinstance(node_value, ScalarNode) and not node_value.style):
            best_style = False
        value.append((node_key, node_value))
    if self.default_flow_style is not None:
        node.flow_style = self.default_flow_style
    else:
        node.flow_style = best_style
    return node
yaml.add_representer(namespace, represent_map)
yaml.add_representer(OrderedDict, represent_map)
yaml.add_representer(six.text_type, getattr(yaml.representer.SafeRepresenter, 'represent_'+six.text_type.__name__))


user_conf = yaml.load(args.config.read())
user_conf.setdefault('mica_opal_url', user_conf.opal_url)
args.config.close()


conf = yaml.load("""
opal:
  url: %(opal_url)r
  username: %(opal_username)r
  password: %(opal_password)r
  projects:
    regression_test_project:
      tables:
        diabetes_antwerp:
          file: Diabetes_Antwerp_export.zip
          user_access: [%(opal_mica_user)r]
      views:
        MDS_Diabetes_Antwerp:
          file: MDS_Diabetes_Antwerp.xml
          tables: [diabetes_antwerp]
          user_access: [%(opal_mica_user)r]

mica:
  url: %(mica_url)r
  username: %(mica_username)r
  password: %(mica_password)r
  studies:
    regressiontest study:
      summary: A dummy used in automated testing
  datasets:
    regressiontest dataset: 
      studies:
        regressiontest study:
          url: %(mica_opal_url)r
          dataset: regression_test_project
          table: MDS_Diabetes_Antwerp
      variables:
        donor_diagnosis_sampling: {taxonomy: icd_10}
      queries:
        Query test 1:
          variables:
            biobank_id: {}
            donor_age_sampling: {range: [300, '']}
            sample_date: {range: ['', 2005-1-1]}
          result:
            regressiontest study:
              Matched: {items: 101, donors: 37}
              biobank_id: {items: 494, donors: 114}
              donor_age_sampling: {items: 162, donors: 55}
              sample_date: {items: 372, donors: 113}
              Total: {items: 494, donors: 114}
        Query test 2:
          variables:
            biobank_id: {}
            donor_diagnosis_sampling: {taxonomy: [E11_x, E14_x]}
            type_sample: {categories: [SER]}
          result:
            regressiontest study:
              Matched: {items: 456, donors: 112}
              biobank_id: {items: 494, donors: 114}
              donor_diagnosis_sampling: {items: 456, donors: 112}
              type_sample: {items: 494, donors: 114}
              Total: {items: 494, donors: 114}
""" % user_conf)

timeout = 3


def newdriver():
    # Create a new instance of the Firefox driver
    driver  = pylenium.Firefox()
    size = driver.window_size
    driver.window_size = (1100, min(1000, size[1]))
    driver.window_position = (0,0)
    # auto-kill the browser on exit
    atexit.register(driver.quit)
    return driver


def test_mica():
    global mica
    browser = mica = Browser(newdriver(), conf.mica.url, HomePage, 
                             username=conf.mica.username, password=conf.mica.password)

    print("page title: " + browser.title)

    assert not browser.loggedIn()
    print("logging in")
    browser.logIn(conf.mica.username, conf.mica.password)
    assert browser.onPage(HomePage)
    assert browser.loggedIn()

    for study, studydata in conf.mica.studies.items():
        print('looking for study '+study)
        browser.gotoStudies()
        if browser.hasStudy(study):
            print('deleting study '+study)
            browser.gotoStudy(study)
            browser.gotoEdit()
            browser.delete()
            browser.gotoStudies()
        print("creating new study "+study)
        create_study(browser, study, studydata.summary)

    for dataset, datasetdata in conf.mica.datasets.items():
        datasetname = dataset+' v1'
        browser.gotoDatasets()
        print("looking for dataset "+datasetname)
        if browser.hasDataset(datasetname):
            print("deleting dataset "+datasetname)
            browser.gotoDataset(datasetname)
            browser.gotoEdit()
            browser.delete()
        print("creating new dataset "+dataset)
        create_dataset(browser, dataset)

        print("Connecting studies to dataset")
        browser.gotoDatasets()
        browser.gotoDataset(datasetname)
        browser.gotoEditStudies()
        for study, connectiondata in datasetdata.studies.items():
            if browser.hasStudy(study):
                browser.deleteStudy(study)
            browser.addStudy(study)
            browser.configureStudy(study, connectiondata.url, connectiondata.dataset, connectiondata.table)
        print("Testing connection")
        browser.testConnections()
        browser.gotoDataset()

        print('Importing variables')
        browser.importVariables()

        print('Configuring variables')
        for variable, variabledata in datasetdata.variables.items():
            browser.gotoVariables()
            browser.gotoVariable(variable)
            browser.gotoEdit()
            browser.setTaxonomy(variabledata.taxonomy)
            browser.save()
            browser.gotoDataset()

        for query, querydata in datasetdata.queries.items():
            print("Creating "+query)
            browser.gotoQueries()
            if browser.hasQuery(query):
                browser.deleteQuery(query)
            browser.createQuery()
            browser.setName(query)
            for variable, args in querydata.variables.items():
                browser.addVariable(variable, **args)
            browser.save()
            browser.gotoQueries()

            print("Running query")
            browser.gotoQuery(query)
            result = browser.result()
            print("result matches expected: "+ str(result == querydata.result))
    return browser


def create_dataset(browser, name):
    assert browser.onPage(Datasets)
    browser.newDataset()
    browser.setName(name)
    browser.setTypeHarmonization()
    browser.setPublished()
    browser.save()

def create_study(browser, name, summary):
    assert browser.onPage(Studies)
    browser.newStudy()
    browser.setName(name)
    browser.setSummary(summary)
    browser.setPublished()
    browser.save()

def remove_mica_data(browser):
    if not browser.loggedIn():
        print("logging in")
        browser.logIn(conf.mica.username, conf.mica.password)

    browser.gotoDatasets()
    for dataset, datasetdata in conf.mica.datasets.items():
        datasetname = dataset+' v1'
        if browser.hasDataset(datasetname):
            print("deleting dataset "+datasetname)
            browser.gotoDataset(datasetname)
            browser.gotoEdit()
            browser.delete()

    browser.gotoStudies()
    for study, studydata in conf.mica.studies.items():
        if browser.hasStudy(study):
            print('deleting study '+study)
            browser.gotoStudy(study)
            browser.gotoEdit()
            browser.delete()
            browser.gotoStudies()



def test_opal():
    global opal
    browser = opal = Browser(newdriver(), conf.opal.url, LoginPage)
    load_opal_data(browser)
    return opal


def load_opal_data(browser):
    browser.ensureLoggedIn(conf.opal.username, conf.opal.password)
    browser.gotoProjects()
    assert browser.onPage(Projects)
    
    for project, projectdata in conf.opal.projects.items():
        if browser.hasProject(project):
            print("project '{}' already exists, skipping project creation".format(project))
        else:
            print("creating project "+project)
            browser.createProject(project)
        assert browser.hasProject(project)

        browser.gotoProject(project)
        tables = browser.tables()
        print("tables: "+', '.join(tables))
        for table, tabledata in projectdata.tables.items():
            if browser.hasTable(table):
                print('removing table '+table)
                browser.removeTable(table)
            print('importing table '+table)
            browser.importTable(os.path.abspath(tabledata.file), table)
            assert browser.hasTable(table)
            print("indexing table "+table)
            browser.gotoTable(table)
            browser.gotoSummary()
            browser.index()
            print("setting permissions on "+table)
            browser.gotoPermissions()
            for user in tabledata.get('user_access', ()):
                if not browser.getPermission(user):
                    browser.addUserPermission(user, 'View dictionary and summaries')
            browser.gotoProject()

        for view, viewdata in projectdata.views.items():
            if browser.hasTable(view):
                print('removing view '+view)
                browser.removeTable(view)
            print("importing view "+view)
            browser.importView(view, viewdata.tables, os.path.abspath(viewdata.file))
            print("indexing view "+view)
            browser.gotoSummary()
            browser.index()
            print("setting permissions on "+view)
            browser.gotoPermissions()
            for user in viewdata.get('user_access', ()):
                if not browser.getPermission(user):
                    browser.addUserPermission(user, 'View dictionary and summaries')
            browser.gotoProject()

def remove_opal_data(browser):
    browser.ensureLoggedIn(conf.opal.username, conf.opal.password)
    for project, projectdata in conf.opal.projects.items():
        print("removing project "+project)
        browser.gotoProjects()
        if not browser.hasProject(project):
            print("project '{}' not found, cannot remove it".format(project))
            return
        browser.gotoProject(project)
        print("removing tables from project "+project)
        browser.removeAllTables()
        browser.removeProject()
    

def main():
    if args.run_opal:
        print('Testing Opal and loading data')
        opal = test_opal()
    if args.run_mica:
        print('Testing Mica')
        mica = test_mica()
    if args.delete:
        if args.run_mica:
            print('Removing Mica data')
            remove_mica_data(mica)
        if args.run_opal:
            print('Removing Opal data')
            remove_opal_data(opal)


if __name__ == '__main__':
    main()
