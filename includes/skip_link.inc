<?php

/**
 * @file
 * Skip Link.
 */

use Drupal\civictheme\CivicthemeConfigManager;

/**
 * HTML pre-process for a Skip Link.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 */
function _civictheme_preprocess_html__skip_link(&$variables) {
  /** @var \Drupal\civictheme\CivicthemeConfigManager $theme_config_manager */
  $theme_config_manager = \Drupal::classResolver(CivicthemeConfigManager::class);

  $variables['skip_link_theme'] = $theme_config_manager->load('components.skip_link.theme');
}
