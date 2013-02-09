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
 * Renders a JSON response of an entire album.
 *
 * @package Json
 * @subpackage UserInterface
 * @author Dean Moses <moses@tacocat.com>
 * @version $Revision$
 */
class SaveCaptionView extends GalleryView {

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
		global $_POST;

		// set JSON-appropriate response headers
		$phpVm = $gallery->getPhpVm();
		if (!$phpVm->headers_sent()) {
		    $phpVm->header('Content-type: application/json; charset=UTF-8');
		    $phpVm->header('Cache-Control: no-cache');
		    $phpVm->header('Pragma: no-cache'); 
		}
		
		// Go no further if user isn't logged in
		list ($ret, $isAnonymousUser) = GalleryCoreApi::isAnonymousUser();
		if ($ret) {
			header(':', true, 500);
			print(json_encode("error: " + $ret->getAsText()));
		   return;
		}
		if ($isAnonymousUser) {
      	header(':', true, 401);
			print(json_encode("unauthorized to modify " + $entityId));
			return;
		}

		// Get the entity's ID from the request
		$entityId = $_POST['id'];

		// Return error if the entity isn't found
		if (!isset($entityId)) {
			trigger_error("no ID", E_USER_WARNING);
			header(':', true, 400);
			print(json_encode("error:  no ID"));
			return;
		}
		
		// Ensure the user has permission to edit this item
		$ret = GalleryCoreApi::assertHasItemPermission($entityId, 'core.edit');
		if ($ret) {
      	header(':', true, 401);
			print(json_encode("unauthorized to modify " + $entityId));
			return;
		}

		// Acquire a write lock on the entity
		list ($ret, $lockId) = GalleryCoreApi::acquireWriteLock($entityId);
		if ($ret) {
			header(':', true, 500);
			print $ret->getAsText(); 
			return; 
		}

		// Retrieve gallery entity by ID
		list ($ret, $entity) = GalleryCoreApi::loadEntitiesById($entityId, 'GalleryItem');
		if ($ret) {
			// Free the lock
			GalleryCoreApi::releaseLocks($lockId);
			header(':', true, 400);
			print(json_encode("error:  could not find entity with ID: " + $entityId));
		    return;
		}
		
		// set new title on the entity if one was sent
		$title = $_POST['title'];
		if (isset($title)) $entity->setTitle($title);
		
		// set new description on the entity if one was sent
		$description = $_POST['description'];
		if (isset($description)) $entity->setDescription($description);
		
		// save the entity
		$saveStatusCode = $entity->save();
		
		// free the lock
		GalleryCoreApi::releaseLocks($lockId);
		
		// return an error if the save failed
		if (isset($saveStatusCode) && isset($saveStatusCode->_errorCode)) {
			header(':', true, 500);
			print(json_encode("error saving: " + $saveStatusCode->getAsText()));
			trigger_error($saveStatusCode->getAsText(), E_USER_WARNING);
		   return;
		}
		
		// send JSON
		print(json_encode('success'));

		return null;
	}
}