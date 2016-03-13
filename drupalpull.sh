#!/usr/bin/env bash

set -e

echo '________ __________ ____ _____________  _____  .____      __________ ____ ___.____    .____    ' 
echo '\______ \\______   \    |   \______   \/  _  \ |    |     \______   \    |   \    |   |    |    '
echo ' |    |  \|       _/    |   /|     ___/  /_\  \|    |      |     ___/    |   /    |   |    |    '
echo ' |    `   \    |   \    |  / |    |  /    |    \    |___   |    |   |    |  /|    |___|    |___ '
echo '/_______  /____|_  /______/  |____|  \____|__  /_______ \  |____|   |______/ |_______ \_______ \ '
echo '        \/       \/                          \/        \/                            \/       \/ '

set -x

# Get drupal root
DRUPAL_ROOT=`drush status | grep 'Drupal root' | sed 's/.*:[ ]*//' | sed 's/ *$//'`
echo $DRUPAL_ROOT

if ! type "drush" > /dev/null; then
  wget --quiet -O - http://ftp.drupal.org/files/projects/drush-7.x-5.9.tar.gz | tar -zxf - -C /usr/local/share
  ln -s /usr/local/share/drush/drush /usr/bin/drush
fi

drush vset maintenance_mode 1


# Patch highly critcal sql injection in drupal core if still using an old drupal version (< 7.32)
# see https://www.drupal.org/SA-CORE-2014-005
test=`drush status | grep 'Drupal version' | python -c "print tuple(raw_input().split(':')[1].strip().split('.')) < ('7','32')"`
if [ "$test" = "True" ]
then
        patch --forward --reject-file=- "$DRUPAL_ROOT/includes/database/database.inc" < "$DRUPAL_ROOT/sites/all/SA-CORE-2014-005-D7.patch" || true
fi


# Undo the ugly hack from 4e918df93e6b6665f45a4fc0364d1d5fe44e5a57 in case it got applied to this installation
patch --forward --reject-file=- --fuzz=0 -p0 -d "$DRUPAL_ROOT" < "$DRUPAL_ROOT/sites/all/search_hack_undo.patch" || true

# PATCH ORIGINAL MICA CODE
yes | cp -Rfv "$DRUPAL_ROOT/sites/all/patch/mica_distribution" "$DRUPAL_ROOT/profiles/"

# ----------------------------- activate common functions
#drush pm-list --pipe --type=module --status=enabled > /tmp/enabledmods
drush pm-list --pipe --type=module --no-core > /tmp/allmods
function isenabled() { grep -Fxq "$1" /tmp/enabledmods  ;}
function isdisabled() { ! grep -Fxq "$1" /tmp/enabledmods  ;}

function ensure_mod() {
    echo "ensure_mod $1";
    if isdisabled $1; then
        if grep -Fxqv "$1" /tmp/allmods; then
            drush --yes dl $1 ;
        fi
        drush --yes en $1 ; 
    fi
}

function ensure_feat() {
    echo "ensure_feat $1";
    if isdisabled $1; then
        drush --yes pm-enable $1 ; 
    fi
}
# ---------------------------------------------------------

# DISABLE these themes and modules to make sure they are never enabled.
isenabled seven && drush --yes pm-disable seven
isenabled bartik && drush --yes pm-disable bartik
isenabled locale && drush --yes dis locale
isenabled project_community && drush --yes dis project_community
isenabled project_dataset && drush --yes dis project_dataset
isenabled sesi_studies && drush --yes dis sesi_studies

# Enable some modules that must be enabled.
isdisabled features && drush --yes pm-enable features
isdisabled strongarm && drush --yes pm-enable strongarm

# Install and enable Features Extra module
if isdisabled fe_block; then
    drush --yes dl features_extra
    drush --yes en fe_block
fi

# Fix problem of missing contact table
cd $DRUPAL_ROOT
TABLEFOUND=`drush sqlq "SELECT count(*) FROM information_schema.TABLES WHERE (TABLE_SCHEMA = 'mica') AND (TABLE_NAME = 'contact');"|sed '1d'`
if [ $TABLEFOUND -eq 0 ]; then
   echo "No contact table found; creating..."
   drush --y dis sesi_contact_form
   drush --y pm-uninstall sesi_contact_form
   drush --y dre contact
   drush en --y sesi_contact_form
else
   echo "Contact table found"
fi

# Enable Rules
isdisabled rules_admin && drush --yes en rules rules_admin

# Install and enable entityreference
drush --yes en entityreference

# Enable Date Popup
isdisabled date_popup && drush --yes en date_popup

# Activate organic groups
drush --yes en og og_ui og_context og_access

# Install and enable og_email
ensure_mod og_email

# disable entityreference_prepopulate
isenabled entityreference_prepopulate_token && drush --yes dis entityreference_prepopulate_token
isenabled entityreference_prepopulate && drush --yes dis entityreference_prepopulate

# Install and enable og_email_blast to send emails to a complete group
# Please note that using the ensure_mod function doesn't work here
# as we need a specific version
if isdisabled og_email_blast; then
    drush --yes dl og_email_blast-7.x-2.x-dev
    drush --yes en og_email_blast
fi
# Install and enable uuid_features module
ensure_mod uuid_features

# Install captcha
if isdisabled captcha; then
    drush --yes dl captcha-7.x-1.2
    drush --yes en captcha image_captcha
fi
ensure_feat sesi_captcha

# Install easy_social module
ensure_mod easy_social

# Install and enable oauth
# This module is required by twitter module

if isdisabled oauth_common; then
    drush --yes dl oauth
    drush --yes en oauth_common
    drush --yes en oauth_common_providerui
fi

# Install and enable twitter module
ensure_mod twitter

# Install htmlmail dependency
if isdisabled mailmime; then
    drush --yes dl htmlmail mailmime mailsystem
    drush --yes en htmlmail mailmime
fi

# Install and enable og_email
ensure_mod og_email

# Install and enable pet
ensure_mod pet

# Install and enable dataset
ensure_mod community_by_dataset

# Enable project features.
if isdisabled sesi_eid_login; then
    ensure_feat sesi_eid_login
    drush --yes pm-disable beididp_button    
fi

ensure_feat sesi_user_registration
ensure_feat sesi_dataset_inheritance 
ensure_feat sesi_dataset_versioning
ensure_feat sesi_dataset_access_form
ensure_feat sesi_vocabulary

ensure_mod sesi_misc_fixes
ensure_mod sesi_addtogroup
ensure_mod sesi_notifyexpiration
ensure_mod sesi_membership_fields
ensure_mod sesi_og_addcontent
ensure_mod sesi_rules
ensure_mod sesi_variable_email
ensure_mod sesi_wildcard_search_processor
ensure_mod view_own_unpublished
ensure_mod sesi_variable_create_breadcrumb
ensure_mod sesi_default_menus

# Download Autologout module dependencies and enable it
drush --yes dl autologout
drush --yes en autologout

#query
ensure_mod query_interface
ensure_mod query_subscription
ensure_feat sesi_my_queries_screen
ensure_mod query_access_rights

#ontologies and vocabularies
ensure_mod query_ontologies
ensure_feat sesi_variable_ontologies
isdisabled query_vocabularies && drush --yes en query_vocabularies

#autologout
ensure_mod autologout
ensure_feat sesi_autologout

#addtogroup
ensure_mod sesi_addtogroup

# Backup first
#drush archive-dump /tmp/micasitebk

# Enable Contact Form
ensure_mod contact
ensure_feat sesi_contact_form

# Views restrictions
isenabled views_access_restrictions || drush --yes en views_access_restrictions

#
ensure_feat sesi_communities_and_files
ensure_feat sesi_views_prefilter_access_restrictions
ensure_feat sesi_user_profile_fields
ensure_feat sesi_default_community
ensure_feat sesi_site_map
ensure_feat sesi_easy_social
#ensure_feat sesi_twitter
ensure_feat sesi_printer_friendly
ensure_feat sesi_expiration_date
ensure_feat sesi_notify_expirations
ensure_feat sesi_og_email
ensure_feat sesi_search_index_immediately
ensure_feat sesi_dataset_redirect
ensure_feat sesi_community_hub
ensure_feat sesi_variable_content_type
ensure_feat sesi_contact_configuration
ensure_feat sesi_wildcard_content_search

#Moderation
ensure_mod og_moderation
ensure_feat sesi_moderation


# UPDATE JQUERY VERSION
drush -y eval "variable_set('jquery_update_jquery_version', strval(1.8));"

# Expandable text
ensure_mod collapse_text
ensure_feat sesi_collapse_text
ensure_mod text_hierarchical

# Statistics
ensure_mod better_statistics
ensure_mod userflow

# og admin role
ensure_mod og_admin_role 

# og prepopulate
ensure_mod prepopulate_group_ref

# Filter search result
ensure_mod og_filter_search
ensure_feat sesi_filter_search

# Custom Field JS 
ensure_mod custom_field_js

# Variable form settings
ensure_feat sesi_variable_form

# Save protection button
ensure_mod save_protection_button

# Users profile
ensure_mod users_profile

# Remove an old content type and some fields.
#drush --yes php-eval "node_type_delete('page');"
#drush field-delete field_news_tags --bundle=news
#drush field-delete field_news_link --bundle=news
 
# Index content for solr
#drush sapi-i ok_sitewide_index 10000 25
#drush sapi-s

drush --yes features-revert-all
drush scr $DRUPAL_ROOT/sites/all/drupalpull.php 
drush cache-clear all
 
# Display list of features to check status manually.
#drush features

# File system permissions
chmod 775 $DRUPAL_ROOT/sites/default/files/private

#rebuild permissions
drush php-eval 'node_access_rebuild();'

drush cc all
drush cron || true

# Run query_ontologies database updates
drush eval '_query_ontologies_update_schema();'

drush vset maintenance_mode 0

#change temporarely the snapshot
REPLACEMENT="version = '7.x-8.2-DEPLOYING'"
sed -i.bak "s/version = .*$/$REPLACEMENT/g" $DRUPAL_ROOT/profiles/mica_distribution/modules/mica/extensions/mica_core/mica_core.info

#try to install xvfb, firefox 
if [ ! -f /usr/bin/xvfb-run ] ; then
    echo "First time installation of virtual X server"
    if [[ $EUID -ne 0 ]]; then
       echo "ERROR!!! This script must be run as root" 
       exit 1
    fi
    yum -y install firefox Xvfb libXfont Xorg
    #yum groupinstall -y development
    yum install -y python-setuptools
fi

#try to install selenium library
installed=`python -c "import selenium"` || installed="NOT EMPTY"
if [ ! -z "$installed" ] ; then
    echo "selenium not installed..trying to install"
    curl https://pypi.python.org/packages/source/s/selenium/selenium-2.44.0.tar.gz | tar xzv
    cd selenium-2.44.0
    python setup.py install
    cd ..
fi

#if selenium user exists, remove it and create a new one
seluser=`drush sqlq "SELECT * from users where name='selenium'"`
if [ ! -z "$seluser" ] ; then
   drush user-cancel --yes selenium 
fi
passwd=`date | md5sum | cut -c1-12`
echo "$passwd" > $DRUPAL_ROOT/selenium.passwd
drush user-create selenium --password="$passwd" --mail="mica+selenium@thehyve.nl"
drush urol administrator selenium

passwd=`cat $DRUPAL_ROOT/selenium.passwd`

if [ -f "$DRUPAL_ROOT/micaurl.ini" ] ; then
   MICAURL=`cat $DRUPAL_ROOT/micaurl.ini`
else
   MICAURL='http://localhost/mica'
fi

echo $MICAURL

echo "Running seldrush"
xvfb-run --server-args="-screen 0, 1024x768x24" python $DRUPAL_ROOT/sites/all/seldrush.py "$MICAURL" selenium "$passwd"

#update version
cd $DRUPAL_ROOT/sites/all
DATE_VER=`date '+%m%d%H%M'`
COMMIT_VER=`git rev-parse --short HEAD`
REPLACEMENT="version = '7.x-8.2-$COMMIT_VER-$DATE_VER'"
sed -i.bak "s/version = .*$/$REPLACEMENT/g" $DRUPAL_ROOT/profiles/mica_distribution/modules/mica/extensions/mica_core/mica_core.info

#refresh
cd $DRUPAL_ROOT
drush vset maintenance_mode 1
drush cc all
# make sure the access grants are up to date
drush eval 'node_access_rebuild();'
drush vset maintenance_mode 0

