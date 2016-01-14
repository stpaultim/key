<?php

/**
 * @file
 * Plugin definition for the None key input plugin.
 */

$plugin = array(
  'label' => t('None'),
  'description' => t("A key input for providers that don't accept a value."),
  'build configuration form' => 'key_input_none_settings_form',
);

/**
 * Settings form for the None key input plugin.
 *
 * @param array $form
 *   The settings form.
 * @param array $form_state
 *   The plugin form state.
 *
 * @return array
 *   The modified settings form.
 */
function key_input_none_settings_form($form, &$form_state) {
  $form['key_value_message'] = array(
    '#markup' => t("The selected key provider does not accept a value. See the provider's description for instructions on how and where to store the key value."),
  );
  return $form;
}
