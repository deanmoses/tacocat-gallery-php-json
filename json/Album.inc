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
class AlbumView extends GalleryView {

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
		global $_GET;

		/* set JSON-appropriate response headers */
		$phpVm = $gallery->getPhpVm();
		if (!$phpVm->headers_sent()) {
		    $phpVm->header('Content-type: application/json; charset=UTF-8');
		    $expires = $gallery->getHttpDate($phpVm->time() - 7*24*3600);
		    $phpVm->header('Cache-Control: no-cache');
		    $phpVm->header('Pragma: no-cache');
		    $phpVm->header('Expires: ' . $expires);	    
		}
	
		// Get the album's path ID from the query string
		$path = $_GET['album'];

		$albumId = JsonHelper::getAlbumIdByPath($path);

		/* get gallery entity by id */
		list ($ret, $entity) = GalleryCoreApi::loadEntitiesById($albumId, 'GalleryEntity');
		if ($ret) {
			print("error getting loadEntitiesById");
		    return $ret;
		}

		$memberData = $this->getReducedItem($entity);

		/* get child IDs */
		list ($ret, $childIds) = GalleryCoreApi::fetchChildItemIds($entity);

		if ($ret) {
		    return $ret;
		}

		/* get child entities */
		list ($ret, $childEntities) = GalleryCoreApi::loadEntitiesById($childIds, 'GalleryEntity');
		if ($ret) {
			print("error loading child entities");
			return $ret;
		}

		$reducedChildEntities = array();
		foreach ($childEntities as $childEntity) {
			$reducedChildEntities[] = $this->getReducedItem($childEntity);
			//$reducedChildEntities[] = $childEntity;
		}

		$memberData['children'] = $reducedChildEntities;

		/* send JSON */
		print(json_encode($memberData));

		return null;
    }


	function getReducedItem($item) {
		global $gallery;
		$urlGenerator =& $gallery->getUrlGenerator();
		$reducedItem = array();
		$reducedItem['id'] = $item->id;
		$reducedItem['title'] = $item->title;
		$reducedItem['description'] = str_replace(array("\r", "\r\n", "\n"), '', html_entity_decode($item->description));
		$reducedItem['keywords'] = $item->keywords;
		$reducedItem['summary'] = $item->summary;
		$reducedItem['pathComponent'] = $item->pathComponent;
		$reducedItem['entityType'] = $item->entityType;
		$reducedItem['creationTimestamp'] = $item->creationTimestamp;
		$reducedItem['ownerId'] = $item->ownerId;
		
		$albumUrl = $urlGenerator->generateUrl(
		    array(
			'view' => 'core.ShowItem',
			'itemId' => $item->id),
		    array(
			'forceSessionId' => false,
			'forceFullUrl' => false));

		$reducedItem['url'] = $albumUrl;

		/* retrieve gallery thumbnail */
		$reducedItem['thumbnail'] = $this->getThumbnail($item->id);
		
		if ($item->entityType == "GalleryPhotoItem") {
			$reducedItem['width'] = $item->width;
			$reducedItem['height'] = $item->height;
			$reducedItem['mimeType'] = $item->mimeType;
			$reducedItem['size'] = $item->size;
			
			list ($ret, $resizes)  = GalleryCoreApi::fetchResizesByItemIds( (array) $childEntity->id  );
			if ($ret) {
				print("error fetchResizesByItemIds");
			    return $ret;
			}
			
			$childEnity['resizes'] = $resizes;
		}
		else if ($item == "GalleryAlbumItem") {
			// Place to get anything specific about albums
		}

		return $reducedItem;
	}

	/**
	 * Get thumbnail info for an item
	 */
	function getThumbnail($itemId) {
		global $gallery;
		/* generate url to the album and rss feed */
		$urlGenerator =& $gallery->getUrlGenerator();
		list ($ret, $thumbArrayl) = GalleryCoreApi::fetchThumbnailsByItemIds(array($itemId));
		if ($ret) {
			return $ret;
		}

		$reducedItem = array();

		if (!empty($thumbArrayl)) {
			$thumb = $thumbArrayl[$itemId];

			$reducedItem['id'] = $thumb->id;
			$reducedItem['width'] = $thumb->width;
			$reducedItem['height'] = $thumb->height;
			$reducedItem['mimeType'] = $thumb->mimeType;

			$imageUrl = $urlGenerator->generateUrl(
			    array(
				'view' => 'core.DownloadItem',
				'itemId' => $thumb->id),
			    array(
				'forceSessionId' => false,
				'forceFullUrl' => true));


			$reducedItem['url'] = $imageUrl;
		}

		return $reducedItem;
	}

	function error($err) {
		$errorResponse = array();
		$errorResponse['error'] = true;
		$errorResponse['message'] = $err;
		
		print(json_encode($errorResponse));
	}
}
?>
