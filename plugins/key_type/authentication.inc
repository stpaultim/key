<?php

/**
 * @file
 * Plugin definition for the Authentication key type.
 */

$plugin = array(
  'label' => t('Authentication'),
  'description' => t('Use for a password or API key. (A generic key type that does not belong to any other defined key type.)'),
  'group' => 'authentication',
  'key value' => array(
    'plugin' => 'textarea_field',
  ),
);
