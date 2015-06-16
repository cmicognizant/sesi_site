<?php
/**
 * @file
 * sesi_communities_and_files.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function sesi_communities_and_files_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function sesi_communities_and_files_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_field_group_info_alter().
 */
function sesi_communities_and_files_field_group_info_alter(&$data) {
  if (isset($data['group_authorization_maelstrom|node|study|form'])) {
    $data['group_authorization_maelstrom|node|study|form']->data['weight'] = 130; /* WAS: 17 */
  }
  if (isset($data['group_authorization_specific|node|study|form'])) {
    $data['group_authorization_specific|node|study|form']->data['weight'] = 129; /* WAS: 16 */
  }
  if (isset($data['group_authorization|node|study|form'])) {
    $data['group_authorization|node|study|form']->data['children']['0'] = 'group_content_access'; /* WAS: 'group_authorization_maelstrom' */
    $data['group_authorization|node|study|form']->data['children']['1'] = 'group_authorization_maelstrom'; /* WAS: 'group_authorization_specific' */
  }
  if (isset($data['group_general_design|node|study|form'])) {
    $data['group_general_design|node|study|form']->parent_name = ''; /* WAS: 'group_methods' */
    unset($data['group_general_design|node|study|form']->data['children']['0']);
    unset($data['group_general_design|node|study|form']->data['children']['1']);
    unset($data['group_general_design|node|study|form']->data['children']['2']);
  }
  if (isset($data['group_general_info|node|study|form'])) {
    $data['group_general_info|node|study|form']->data['children']['4'] = 'og_group_ref'; /* WAS: 'field_networks' */
  }
}

/**
 * Implements hook_default_search_api_index_alter().
 */
function sesi_communities_and_files_default_search_api_index_alter(&$data) {
  if (isset($data['dataset_index'])) {
    $data['dataset_index']->options['fields']['og_group_ref'] = array(
      'type' => 'list<integer>',
      'entity_type' => 'node',
    ); /* WAS: '' */
  }
  if (isset($data['fields_description_index'])) {
    $data['fields_description_index']->options['fields']['og_group_ref'] = array(
      'type' => 'list<integer>',
      'entity_type' => 'node',
    ); /* WAS: '' */
  }
  if (isset($data['studies_index'])) {
    $data['studies_index']->options['fields']['og_group_ref'] = array(
      'type' => 'list<integer>',
      'entity_type' => 'node',
    ); /* WAS: '' */
  }
  if (isset($data['variable_index'])) {
    $data['variable_index']->options['fields']['og_group_ref'] = array(
      'type' => 'list<integer>',
      'entity_type' => 'node',
    ); /* WAS: '' */
  }
}
  
  
/**
 * Implements hook_field_default_field_instances_alter().
 */
function sesi_communities_and_files_field_default_field_instances_alter(&$data) {
  if (isset($data['og_membership-og_membership_type_default-field_mail_sent'])) {
    $data['og_membership-og_membership_type_default-field_mail_sent']['required'] = 0; /* WAS: 1 */
  }
}

/**
 * Implements hook_strongarm_alter().
 */
function sesi_communities_and_files_strongarm_alter(&$data) {
  if (isset($data['content_access_dataset'])) {
    $data['content_access_dataset']->value['priority'] = -1; /* WAS: '' */
  }
  if (isset($data['content_access_study'])) {
    $data['content_access_study']->value['priority'] = -1; /* WAS: '' */
  }
  if (isset($data['content_access_variable'])) {
    $data['content_access_variable']->value['priority'] = -1; /* WAS: '' */
  }
}

/**
 * Implements hook_views_default_views_alter().
 */
function sesi_communities_and_files_views_default_views_alter(&$data) {
  if (isset($data['studies_search'])) {
    $data['studies_search']->display['default']->display_options['query']['options']['search_api_bypass_access'] = 1; /* WAS: '' */
  }
}

/**
 * Implements hook_node_info().
 */
function sesi_communities_and_files_node_info() {
  $items = array(
    'community' => array(
      'name' => t('Community'),
      'base' => 'node_content',
      'description' => t('CMI Community'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'community_document' => array(
      'name' => t('Community document'),
      'base' => 'node_content',
      'description' => t('CMI Community document'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}