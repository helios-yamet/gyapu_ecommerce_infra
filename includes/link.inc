<?php

/**
 * @file
 * Link related functions.
 */

use Drupal\Component\Utility\Html;

/**
 * Process HTML links within an HTML content string.
 *
 * @param string $html
 *   The string to be processed.
 * @param string $base_url
 *   The hostname to process.
 * @param bool $new_window
 *   Flag to open all links in the new window.
 * @param bool $external_new_window
 *   Flag to open external links in the new window.
 * @param array $override_domains
 *   Array of override domains.
 *
 * @return string
 *   The processed HTML string.
 *
 * @SuppressWarnings(PHPMD.CyclomaticComplexity)
 * @SuppressWarnings(PHPMD.NPathComplexity)
 * @SuppressWarnings(PHPMD.StaticAccess)
 * @SuppressWarnings(PHPMD.BooleanArgumentFlag)
 */
function _civictheme_process_html_content_links($html, $base_url, $new_window = FALSE, $external_new_window = FALSE, array $override_domains = []) {
  if (_civictheme_feature_is_optedout('process', 'components.link')) {
    return $html;
  }

  $class_prefix = 'ct-';
  $link_class = 'ct-content-link ct-theme-light';
  $link_external_class = 'ct-content-link--external';

  $html = _civictheme_process_html_content_links_emails($html);

  $dom = Html::load($html);

  if (!$dom) {
    return $html;
  }

  $anchors = $dom->getElementsByTagName('a');
  if (!$anchors->count()) {
    return $html;
  }

  foreach ($anchors as $anchor) {
    // Apply only to anchors with links.
    if (!$anchor->hasAttribute('href')) {
      continue;
    }

    // Apply only if there are no classes prefixed with a prefix.
    $classes = $anchor->hasAttribute('class') ? ($anchor->getAttribute('class') ?: '') : '';

    $url = $anchor->getAttribute('href');
    $is_external = civictheme_url_is_external($url, $base_url, $override_domains);

    if ((!str_contains($classes, $class_prefix)) && (!str_contains($classes, $link_class))) {
      $classes .= ' ' . $link_class;
    }

    if ($is_external && !str_contains($classes, $link_external_class)) {
      $classes .= ' ' . $link_external_class;
    }

    if ($new_window || ($is_external && $external_new_window)) {
      if (!$anchor->hasAttribute('target')) {
        $anchor->setAttribute('target', '_blank');
      }
      if (!$anchor->hasAttribute('aria-label')) {
        $anchor->setAttribute('aria-label', 'Opens in a new tab');
      }
      $span = $dom->createElement('span', '(Opens in a new tab/window)');
      $span->setAttribute('class', 'ct-visually-hidden');
      $anchor->appendChild($span);
      // External icon.
      $svg = $dom->createElement('svg');
      $svg->setAttribute('class', 'ct-icon');
      $svg->setAttribute('aria-hidden', 'true');
      $svg->setAttribute('role', 'img');
      $svg->setAttribute('width', '24');
      $svg->setAttribute('height', '24');
      $svg->setAttribute('viewBox', '0 0 24 24');
      $svg->setAttribute('xmlns', 'http://www.w3.org/2000/svg');
      $path = $dom->createElement('path', '');
      $path->setAttribute('d', 'M17.9199 6.62C17.8185 6.37565 17.6243 6.18147 17.3799 6.08C17.2597 6.02876 17.1306 6.00158 16.9999 6H6.99994C6.73472 6 6.48037 6.10536 6.29283 6.29289C6.1053 6.48043 5.99994 6.73478 5.99994 7C5.99994 7.26522 6.1053 7.51957 6.29283 7.70711C6.48037 7.89464 6.73472 8 6.99994 8H14.5899L6.28994 16.29C6.19621 16.383 6.12182 16.4936 6.07105 16.6154C6.02028 16.7373 5.99414 16.868 5.99414 17C5.99414 17.132 6.02028 17.2627 6.07105 17.3846C6.12182 17.5064 6.19621 17.617 6.28994 17.71C6.3829 17.8037 6.4935 17.8781 6.61536 17.9289C6.73722 17.9797 6.86793 18.0058 6.99994 18.0058C7.13195 18.0058 7.26266 17.9797 7.38452 17.9289C7.50638 17.8781 7.61698 17.8037 7.70994 17.71L15.9999 9.41V17C15.9999 17.2652 16.1053 17.5196 16.2928 17.7071C16.4804 17.8946 16.7347 18 16.9999 18C17.2652 18 17.5195 17.8946 17.707 17.7071C17.8946 17.5196 17.9999 17.2652 17.9999 17V7C17.9984 6.86932 17.9712 6.74022 17.9199 6.62Z');
      $svg->appendChild($path);
      $anchor->appendChild($svg);
    }

    $anchor->setAttribute('class', trim($classes));
  }

  return Html::serialize($dom);
}

/**
 * Process HTML content to replace emails with mailto links.
 *
 * @SuppressWarnings(PHPMD.StaticAccess)
 * @SuppressWarnings(PHPMD.MissingImport)
 */
function _civictheme_process_html_content_links_emails($html) {
  if (_civictheme_feature_is_optedout('process', 'components.link.email')) {
    return $html;
  }

  $dom = Html::load($html);

  if (!$dom) {
    return $html;
  }

  $xpath = new \DOMXPath($dom);

  $text_nodes = $xpath->query('//text()[not(ancestor::a)]');

  foreach ($text_nodes as $text_node) {
    if (preg_match_all(_civictheme_process_html_content_links_get_email_regex(), $text_node->nodeValue, $matches)) {
      $emails = $matches[0];
      foreach ($emails as $email) {
        $replacement = $dom->createDocumentFragment();
        $replacement->appendXML(str_replace($email, "<a href=\"mailto:$email\">$email</a>", $text_node->textContent));
        $text_node->replaceWith($replacement);
      }
    }
  }

  return Html::serialize($dom);
}

/**
 * Email pattern as a regex string.
 */
function _civictheme_process_html_content_links_get_email_regex() {
  return '/\b[A-Za-z0-9._%+-]+@(?:(?:[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?\.)+(?:[a-zA-Z]{2,})\b)(?!\.[a-zA-Z0-9]{1,}\b)/';
}
