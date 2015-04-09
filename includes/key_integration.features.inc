<?php

/**
 * @file
 * Provides Features integration for managing key integrations.
 */

/**
 * Implements hook_features_export_options().
 */
function key_integration_features_export_options() {
  return key_get_integrations_as_options(NULL, NULL, TRUE);
}

/**
 * Implements hook_features_export().
 */
function key_integration_features_export($data, &$export, $module_name) {
  $pipe = array();

  $export['dependencies']['key'] = 'key';
  foreach ($data as $component) {
    $export['features']['key_integration'][$component] = $component;
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function key_integration_features_export_render($module_name, $data, $export = NULL) {
  $code = array();
  $code[] = '  $integrations = array();';
  $code[] = '';

  foreach ($data as $name) {
    $integration = key_get_integration_settings($name, TRUE);
    $integration_identifier = features_var_export($name);
    $integration_export = features_var_export($integration , '  ');
    $code[] = "  // Exported key integration: {$name}.";
    $code[] = "  \$integrations[{$integration_identifier}] = {$integration_export};";
    $code[] = "";
  }

  $code[] = '  return $integrations;';
  $code = implode("\n", $code);

  return array('key_default_integrations' => $code);
}

/**
 * Implements hook_features_revert().
 */
function key_integration_features_revert($module) {
  key_integration_features_rebuild($module);
}

/**
 * Implements hook_features_rebuild().
 */
function key_integration_features_rebuild($module) {
  $integrations = module_invoke($module, 'key_default_integrations');

  foreach ($integrations as $key => $integration) {
    key_save_integration_settings($integration);
  }
}
