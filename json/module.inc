<?php
/*
 * Gallery - a web based photo album viewer and editor
 * Copyright (C) 2000-2008 Bharat Mediratta
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

/**
 * JSON API module
 *
 * @package Json
 * @author Dean Moses <http://tacocat.com>
 * @version $Revision: 1 $
 */
class JsonModule extends GalleryModule {

    function JsonModule() {
	global $gallery;

	$this->setId('json');
	$this->setName($gallery->i18n('JSON'));
	$this->setDescription($gallery->i18n('JSON API'));
	$this->setVersion('0.0.1');
	$this->_templateVersion = 1;
	$this->setGroup('export', $gallery->i18n('Export'));
	$this->setCallbacks('getSiteAdminViews|getItemLinks');
	$this->setRequiredCoreApi(array(7, 34));
	$this->setRequiredModuleApi(array(3, 6));
    }
}
?>
