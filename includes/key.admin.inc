<?php

/**
 * @file
 * This file holds the functions for the Key admin settings.
 *
 * @ingroup key
 */

function key_list() {
  return "<h1>Key List</h1>";
}

function key_config_form($form, &$form_state, $config = NULL) {

  $form['label'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $config['label'],
    '#description' => t('The human-readable name of the key.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $config['name'],
    '#maxlength' => 32,
    '#disabled' => isset($config['name']),
    '#machine_name' => array(
      'exists' => 'key_config_load',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for the key. It must only contain lowercase letters, numbers, and underscores.'),
  );
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $config['description'],
    '#description' => t('A short description of the key.'),
  );

  return $form;
}
