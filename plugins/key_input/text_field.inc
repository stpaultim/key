<?php

/**
 * @file
 * Plugin definition for the Text Field key input plugin.
 */

$plugin = array(
  'label' => t('Text field'),
  'description' => t("A simple text field."),
  'default configuration' => 'key_input_text_field_default_configuration',
  'build configuration form' => 'key_input_text_field_build_configuration_form',
);

/**
 * The defaults for the Text Field key input settings form.
 *
 * @return array
 *   The default settings.
 */
function key_input_text_field_default_configuration() {
  return array(
    'key_value' => '',
  );
}

/**
 * Settings form for the Text Field key input plugin.
 *
 * @param array $form
 *   The settings form.
 * @param array $form_state
 *   The plugin form state.
 *
 * @return array
 *   The modified settings form.
 */
function key_input_text_field_build_configuration_form($form, &$form_state) {
  $key_value_data = $form_state['key_value'];

  $form['key_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Key value'),
    '#required' => TRUE,
    '#default_value' => $key_value_data['current'],
    // Tell the browser not to autocomplete this field.
    '#attributes' => ['autocomplete' => 'off'],
  );

  return $form;
}
