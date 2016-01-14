<?php

/**
 * @file
 * Plugin definition for the Basic key type.
 */

$plugin = array(
  'label' => t('Basic'),
  'description' => t('A Basic key is one that is not logically associated with any other defined type.'),
  'key value' => array(
    'plugin' => 'text_field'
  ),
);

