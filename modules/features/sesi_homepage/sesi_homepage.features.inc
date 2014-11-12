<?php
/**
 * @file
 * sesi_homepage.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function sesi_homepage_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "panels_mini" && $api == "panels_default") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function sesi_homepage_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}