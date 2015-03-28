<?php

/**
 * @file
 * Plugin definition for the Stripe payment method.
 */

$plugin = array(
  'title' => t('Stripe'),
  'type' => 'commerce_payment',
  'key_fields' => array('secret_key'),
);
