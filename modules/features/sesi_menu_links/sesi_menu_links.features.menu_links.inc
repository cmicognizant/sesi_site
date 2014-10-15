<?php
/**
 * @file
 * sesi_menu_links.features.menu_links.inc
 */

/**
 * Implements hook_menu_default_menu_links().
 */
function sesi_menu_links_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: main-menu:<firstchild>
  $menu_links['main-menu:<firstchild>'] = array(
    'menu_name' => 'main-menu',
    'link_path' => '<firstchild>',
    'router_path' => '<firstchild>',
    'link_title' => 'Data Access',
    'options' => array(
      'alter' => TRUE,
      'unaltered_hidden' => 1,
    ),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 1,
    'weight' => 20,
  );
  // Exported menu link: main-menu:<front>
  $menu_links['main-menu:<front>'] = array(
    'menu_name' => 'main-menu',
    'link_path' => '<front>',
    'router_path' => '',
    'link_title' => 'Home',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 1,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 0,
  );
  // Exported menu link: main-menu:blog
  $menu_links['main-menu:blog'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'blog',
    'router_path' => 'blog',
    'link_title' => 'Blogs',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 25,
    'parent_path' => 'node/5',
  );
  // Exported menu link: main-menu:calendar-field-event-date/year
  $menu_links['main-menu:calendar-field-event-date/year'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'calendar-field-event-date/year',
    'router_path' => 'calendar-field-event-date/year',
    'link_title' => 'Calendar',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 0,
    'parent_path' => 'events',
  );
  // Exported menu link: main-menu:data-access/admin
  $menu_links['main-menu:data-access/admin'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'data-access/admin',
    'router_path' => 'data-access/admin',
    'link_title' => 'Data Access Administration',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 10,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: main-menu:data-access/approved
  $menu_links['main-menu:data-access/approved'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'data-access/approved',
    'router_path' => 'data-access/approved',
    'link_title' => 'DACO Approved Projects',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 1,
    'parent_path' => 'node/3',
  );
  // Exported menu link: main-menu:data-access/committee
  $menu_links['main-menu:data-access/committee'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'data-access/committee',
    'router_path' => 'data-access/committee',
    'link_title' => 'Data Access Committee',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 15,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: main-menu:data-access/request
  $menu_links['main-menu:data-access/request'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'data-access/request',
    'router_path' => 'data-access/request',
    'link_title' => 'Data Access Request',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 20,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: main-menu:data-access/requests
  $menu_links['main-menu:data-access/requests'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'data-access/requests',
    'router_path' => 'data-access/requests',
    'link_title' => 'My Data Access Requests',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 25,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: main-menu:datasets
  $menu_links['main-menu:datasets'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'datasets',
    'router_path' => 'datasets',
    'link_title' => 'Datasets',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 5,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: main-menu:events
  $menu_links['main-menu:events'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'events',
    'router_path' => 'events',
    'link_title' => 'Events',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 1,
    'expanded' => 1,
    'weight' => 15,
    'parent_path' => 'node/5',
  );
  // Exported menu link: main-menu:forum
  $menu_links['main-menu:forum'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'forum',
    'router_path' => 'forum',
    'link_title' => 'Forums',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 20,
    'parent_path' => 'node/5',
  );
  // Exported menu link: main-menu:news
  $menu_links['main-menu:news'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'news',
    'router_path' => 'news',
    'link_title' => 'News',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 10,
    'parent_path' => 'node/5',
  );
  // Exported menu link: main-menu:node/2
  $menu_links['main-menu:node/2'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'node/2',
    'router_path' => 'node/%',
    'link_title' => 'Resources',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 1,
    'expanded' => 1,
    'weight' => 10,
  );
  // Exported menu link: main-menu:node/3
  $menu_links['main-menu:node/3'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'node/3',
    'router_path' => 'node/%',
    'link_title' => 'Research',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 1,
    'expanded' => 1,
    'weight' => 30,
  );
  // Exported menu link: main-menu:node/4
  $menu_links['main-menu:node/4'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'node/4',
    'router_path' => 'node/%',
    'link_title' => 'About',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 50,
  );
  // Exported menu link: main-menu:node/5
  $menu_links['main-menu:node/5'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'node/5',
    'router_path' => 'node/%',
    'link_title' => 'Community',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 1,
    'expanded' => 1,
    'weight' => 45,
  );
  // Exported menu link: main-menu:node/6
  $menu_links['main-menu:node/6'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'node/6',
    'router_path' => 'node/%',
    'link_title' => 'Consortium Policies and Guidelines',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 5,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: main-menu:projects
  $menu_links['main-menu:projects'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'projects',
    'router_path' => 'projects',
    'link_title' => 'Projects',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 2,
    'parent_path' => 'node/3',
  );
  // Exported menu link: main-menu:publications
  $menu_links['main-menu:publications'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'publications',
    'router_path' => 'publications',
    'link_title' => 'Publications',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 10,
    'parent_path' => 'node/3',
  );
  // Exported menu link: main-menu:studies
  $menu_links['main-menu:studies'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'studies',
    'router_path' => 'studies',
    'link_title' => 'Studies',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 1,
    'expanded' => 0,
    'weight' => 5,
  );
  // Exported menu link: main-menu:studies-search
  $menu_links['main-menu:studies-search'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'studies-search',
    'router_path' => 'studies-search',
    'link_title' => 'Search Studies',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 10,
    'parent_path' => 'studies',
  );
  // Exported menu link: main-menu:variable-search
  $menu_links['main-menu:variable-search'] = array(
    'menu_name' => 'main-menu',
    'link_path' => 'variable-search',
    'router_path' => 'variable-search',
    'link_title' => 'Variables Search',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 1,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 10,
    'parent_path' => '<firstchild>',
  );
  // Exported menu link: management:admin/config/development/maintenance
  $menu_links['management:admin/config/development/maintenance'] = array(
    'menu_name' => 'management',
    'link_path' => 'admin/config/development/maintenance',
    'router_path' => 'admin/config/development/maintenance',
    'link_title' => 'Maintenance mode',
    'options' => array(
      'attributes' => array(
        'title' => 'Take the site offline for maintenance or bring it back online.',
      ),
    ),
    'module' => 'system',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => -10,
    'parent_path' => 'admin/config/development',
  );
  // Exported menu link: management:admin/structure/menu/manage/main-menu
  $menu_links['management:admin/structure/menu/manage/main-menu'] = array(
    'menu_name' => 'management',
    'link_path' => 'admin/structure/menu/manage/main-menu',
    'router_path' => 'admin/structure/menu/manage/%',
    'link_title' => 'Main menu',
    'options' => array(),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => 0,
    'parent_path' => 'admin/structure/menu',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('About');
  t('Blogs');
  t('Calendar');
  t('Community');
  t('Consortium Policies and Guidelines');
  t('DACO Approved Projects');
  t('Data Access');
  t('Data Access Administration');
  t('Data Access Committee');
  t('Data Access Request');
  t('Datasets');
  t('Events');
  t('Forums');
  t('Home');
  t('Main menu');
  t('Maintenance mode');
  t('My Data Access Requests');
  t('News');
  t('Projects');
  t('Publications');
  t('Research');
  t('Resources');
  t('Search Studies');
  t('Studies');
  t('Variables Search');


  return $menu_links;
}