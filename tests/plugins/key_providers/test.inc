<?php

/**
 * @file
 * An example key provider just for tests.
 *
 * @ingroup key
 */

$plugin = array(
  'title' => t('Test Key Provider'),
  'description' => t('This is just a test key provider.'),
  'encrypt callback' => 'key_test_key_providers_test',
);

/**
 * Callback for encryption.
 * We'll just return the text, since we're not concerned with testing encryption.
 */
function key_test_key_providers_test($settings) {
  return 'Some long key';
}
