from collections import OrderedDict

def css_escape(string, quotes="'"):
    return string.replace(quotes, '\\'+quotes)

def xpath_string_escape(s):
    if "'" not in s: return "'%s'" % s
    if '"' not in s: return '"%s"' % s
    return "concat('%s')" % s.replace("'", "',\"'\",'")

def contains(tag='*', text=None, prefix='//', class_=None, function='contains'):
    c = ''
    if class_ is not None:
        c = xpath_class(class_)+' and '
    return '''%s%s[%s.//text()[%s(., %s)]]''' % (prefix, tag, c, function, xpath_string_escape(text))

def xpath_class(cls):
    if not cls:
        return '.'
    return '''contains(concat(' ',normalize-space(@class),' '),%s)''' % xpath_string_escape(' '+cls+' ')

class namespace(OrderedDict):

    def __getattr__(self, attr):
        # Make sure we don't mess with OrderedDict's internals (private variables)
        if attr.startswith('_OrderedDict_'):
            return object.__getattr__(self, attr)
        return self[attr]

    def __setattr__(self, attr, data):
        if attr.startswith('_OrderedDict_'):
            return object.__setattr__(self, attr, data)
        self[attr] = data

    def __delattr__(self, attr):
        if attr.startswith('_OrderedDict_'):
            return object.__delattr_(self, attr)
        del self[attr]
