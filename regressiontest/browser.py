from functools import partial

from pylenium import Pylenium
from pylenium import conditions as C

class Browser(object):
    def __init__(self, driver, url, page, username=None, password=None):
        self.username = username
        self.password = password
        self.driver = driver
        driver.get(url)
        self.page = page(driver, browser=self)
        self.page.waitFor()

    def close(self):
        self.driver.close()

    def logIn(self, username, password):
        self.username = username
        self.password = password
        return self._callPageMethod('logIn', username, password)

    @property
    def title(self):
        return self.page.title

    def __getattr__(self, attr):
        # check if attribute exists
        getattr(self.page, attr)
        return partial(self._callPageMethod, attr)

    def _callPageMethod(self, method, *args, **kwargs):
        retval = getattr(self.page, method)(*args, **kwargs)
        if isinstance(retval, Page):
            self.page = retval
            retval.browser = self.browser
        return retval

    def onPage(self, page):
        return page.onPage(self.page)

    def __dir__(self):
        return self.__dict__.keys() + dir(self.page)
        


class _wrapper(object):
    def __init__(self, browser, method, *extra_args):
        self.browser = browser
        self.method = method
        self.extra_args = extra_args

    def __call__(self, *args, **kwargs):
        retval = getattr(self.browser.page, self.method)(*(self.extra_args + args), **kwargs)
        if isinstance(retval, Page):
            self.browser.page = retval
        return retval
        

class Page (object):
    def __init__(self, driver=None, browser=None):
        if driver:
            assert isinstance(driver, Pylenium)
        if browser:
            assert isinstance(browser, Browser)
        self._driver = driver
        self.browser = browser
    
    @property
    def driver(self):
        """Allow using a global for now to prevent threading the driver through all objects. 
        This should really be a dynamic scoped variable."""
        #global driver
        return self._driver or driver

    @property
    def title(self):
        return self.driver.title

    def waitFor(self, timeout=None):
        self.driver.wait_until(lambda _: self.onPage(), timeout=timeout)
        return self

    def waitForCondition(self, *conditions, **kwargs):
        kwargs.setdefault('timeout', 10)
        conds = (c.test if isinstance(c, C.Condition) else lambda _, c=c: c() for c in conditions)
        return self.driver.wait_until(*conds, **kwargs)

    def elementExists(self, *args, **kwargs):
        return C.element_present(*args, **kwargs).test(self.driver)

    def findElement(self, *args, **kwargs):
        return self.driver.find_element(*args, **kwargs)

    def clickLink(self, text):
        self.findElement(link_text=text).click()
