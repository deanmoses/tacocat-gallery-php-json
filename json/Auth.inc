<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2006 Bharat Mediratta
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA  02110-1301, USA.
 */
GalleryCoreApi::requireOnce('modules/json/classes/JsonHelper.class');

/**
 * Returns whether the user is logged in and a system administrator.
 *
 * @package Json
 * @subpackage UserInterface
 * @author Dean Moses <moses@tacocat.com>
 * @version $Revision$
 */
class AuthView extends GalleryView {

    /**
     * @see GalleryView::isImmediate
     */
    function isImmediate() {
		return true;
    }

    /**
     * @see GalleryView::renderImmediate
     */
    function renderImmediate($status, $error) {
		global $gallery;

		// set JSON-appropriate response headers
		$phpVm = $gallery->getPhpVm();
		if (!$phpVm->headers_sent()) {
		    $phpVm->header('Content-type: application/json; charset=UTF-8');
		    $phpVm->header('Cache-Control: no-cache');
		    $phpVm->header('Pragma: no-cache'); 
		}
		
		// The response object that will be returned as JSON
		$response = array();

		$galleryStatus = GalleryCoreApi::assertUserIsSiteAdministrator();

		
		if ($galleryStatus && $galleryStatus->getErrorCode()) {
			$response['authenticated'] = false;
		}
		else {
			$response['authenticated'] = true;
		}
		
		print(json_encode($response));
		
		return null;
	}
}