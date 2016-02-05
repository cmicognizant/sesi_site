#!/usr/bin/env python
from __future__ import print_function

"""
Example script to store generated reporting files in a secure path

A script like this can be set up to be run from cron. It will check for manifest files in the manifests_path directory.
If a file named in the manifest exists in source_path, it will be moved to a randomly named subdirectory in dest_path
and emails will be sent to the system administrator and the owner of the report with a link to the moved file.

See the demoreport_manifest.info file for an example and more explanation of what the manifest files should look like.

"""

####### Configuration settings ########

# New reports are moved from this directory ...
source_path = '/var/www/reports'

# ... to this directory
dest_path = '/var/www/html/reports'

# Manifests are looked up in this directory
manifests_path = '/var/www/html/reports/manifests'

# The admin always receives an email with the url
admin_email = 'admin@cmi-vzw.be'

# the sender address for notification mails
mail_from = 'info@cmi-vzw.be'

# Base url to which the key and report name are appended
url_base = 'https://mica.cmi-vzw.be/reports/'

# Mail server used to send emails
smtp_server = 'localhost'

#######################################


import sys
import os
import yaml
import binascii
import shutil
from textwrap import dedent
import smtplib
from email.mime.text import MIMEText


def load_info(reportinfo):
    path = os.path.join(manifests_path, reportinfo)
    if not os.path.exists(path):
        raise ConfigurationException("Manifest for {} not found".format(reportinfo))
    with open(path) as m:
        config = yaml.load(m.read())
    if not {'name', 'owner', 'duplicates'}.issubset(config):
        raise ConfigurationException("Manifest {} must contain keys 'name', 'owner' and 'duplicates'"
                                     .format(reportinfo))
    if not config['duplicates'] in ('new_key', 'overwrite'):
        raise ConfigurationException("Key 'duplicates' in {} must have a value of 'new_key' or 'overwrite'".
                                     format(reportinfo))
    if not type(config.get('overwrite_notify', 'yes')) == bool:
        raise ConfigurationException("Key 'overwrite_notify' in {} must have a value of yes, true, no or false"
                                     .format(reportinfo))
    return config

def find_existing(report, _cache={}):
    if report in _cache:
        return _cache[report]
    key = None
    ctime = 0
    for d in os.listdir(dest_path):
        path = os.path.join(dest_path, d, report)
        if os.path.exists(path):
            new_ctime = os.path.getctime(path)
            if new_ctime >= ctime:
                ctime = new_ctime
                key = d
    _cache[report] = key
    return key

def move_file(config):
    report = config['name']
    if config['duplicates'] == 'new_key' or not find_existing(report):
        key = binascii.b2a_hex(os.urandom(16))
        mail = True
    elif config['duplicates'] == 'overwrite':
        key = find_existing(report)
        mail = config.get('overwrite_notify', False)

    destdir = os.path.join(dest_path, key)
    destfile = os.path.join(destdir, report)
    sourcefile = os.path.join(source_path, report)

    if not os.path.exists(destdir):
        os.mkdir(destdir)
        mail = True

    try:
        os.rename(sourcefile, destfile)
    except OSError:
        try:
            os.remove(destfile)
        except OSError:
            shutil.rmtree(destfile, ignore_errors=True)
        shutil.move(sourcefile, destdir)

    if mail:
        send_mail(config, key)

    
def send_mail(config, key):
    report = config['name']
    link = url_base + key + '/' + report

    msg = MIMEText(dedent(
        """
        Dear user,

        Report {report} is ready at {link}.

        This notification has been automatically generated by Mica.
        """).format(report=report, link=link))

    msg['Subject'] = 'Report {} available'.format(report)
    msg['From'] = mail_from

    smtp = smtplib.SMTP(smtp_server)
    recepients = config['owner']
    recepients = [recepients] if type(recepients) != list else recepients
    for to in [admin_email] + recepients:
        del msg['To']
        msg['To'] = to
        smtp.sendmail(mail_from, [to], msg.as_string())
    smtp.quit()
    

def main():
    error = False
    count = 0
    for reportinfo in os.listdir(manifests_path):
       try:
            if reportinfo.startswith('.'):
                continue
            config = load_info(reportinfo)
            if not os.path.exists(os.path.join(source_path, config['name'])):
                continue
            move_file(config)
            count += 1
       except (OSError, IOError, ConfigurationException) as e:
            print('Error processing {}: '.format(reportinfo)+str(e), file=sys.stderr)
            error = True
    print("moved {} reports".format(count), file=sys.stderr)
    if error:
        sys.exit(1)


class ConfigurationException (Exception):
    pass


if __name__ == '__main__':
    main()
