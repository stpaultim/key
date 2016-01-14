<?php

/**
 * @file
 * Plugin definition for the Text Field key input plugin.
 */

$plugin = array(
  'label' => t('Text field'),
  'description' => t("A simple text field."),
  'build configuration form' => NULL,
);

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
function key_input_none_settings_form($form, &$form_state) {
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
