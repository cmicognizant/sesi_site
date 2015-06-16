<?php
/**
 * @file
 * sesi_communities_and_files.features.features_overrides.inc
 */

/**
 * Implements hook_features_override_default_overrides().
 */
function sesi_communities_and_files_features_override_default_overrides() {
  // This code is only used for UI in features. Exported alters hooks do the magic.
  $overrides = array();

  // Exported overrides for: field_group
  $overrides["field_group.group_authorization_maelstrom|node|study|form.data|weight"] = 130;
  $overrides["field_group.group_authorization_specific|node|study|form.data|weight"] = 129;
  $overrides["field_group.group_authorization|node|study|form.data|children|0"] = 'group_content_access';
  $overrides["field_group.group_authorization|node|study|form.data|children|1"] = 'group_authorization_maelstrom';
  $overrides["field_group.group_general_design|node|study|form.data|children|0"]["DELETED"] = TRUE;
  $overrides["field_group.group_general_design|node|study|form.data|children|1"]["DELETED"] = TRUE;
  $overrides["field_group.group_general_design|node|study|form.data|children|2"]["DELETED"] = TRUE;
  $overrides["field_group.group_general_design|node|study|form.parent_name"] = '';
  $overrides["field_group.group_general_info|node|study|form.data|children|4"] = 'og_group_ref';

  // Exported overrides for: field_instance
  $overrides["field_instance.og_membership-og_membership_type_default-field_mail_sent.required"] = 0;

  // Exported overrides for: variable
  $overrides["variable.content_access_dataset.value|priority"] = -1;
  $overrides["variable.content_access_study.value|priority"] = -1;
  $overrides["variable.content_access_variable.value|priority"] = -1;

  // Exported overrides for: views_view
  $overrides["views_view.studies_search.display|default|display_options|query|options|search_api_bypass_access"] = 1;

  
  
  
  // Exported overrides for: search_api_index
  $overrides["search_api_index.dataset_index.options|fields|og_group_ref"] = array(
    'type' => 'list<integer>',
    'entity_type' => 'node',
  );
  $overrides["search_api_index.fields_description_index.options|fields|og_group_ref"] = array(
    'type' => 'list<integer>',
    'entity_type' => 'node',
  );
  $overrides["search_api_index.studies_index.options|fields|og_group_ref"] = array(
    'type' => 'list<integer>',
    'entity_type' => 'node',
  );
  $overrides["search_api_index.variable_index.options|fields|og_group_ref"] = array(
    'type' => 'list<integer>',
    'entity_type' => 'node',
  );
  
 return $overrides;
}