<?php

function bindQueryCondition($param1, $param2) {
    global $language, $base_url, $photo_query_condition;

    $field_name = '';
    if ($param1 == 'ghs') {
        $field_name = 'h_site_id';
    }

    $param2_filtered = (int) (filtertext(strip_tags(base64_decode($param2))));

    if (isset($field_name) && $field_name != '') {
        $photo_query_condition = " AND " . $field_name . " = '" . $param2_filtered . "'";
    } else {
        $photo_query_condition = '';
    }
    return $field_name;
}

/* On Load more time execute this functions */

function getphotos_reference_photos() {

    global $language, $base_path, $base_url;

    if(isset($_SESSION['active_lang'])){
       $language->language = $_SESSION['active_lang'] ;
    }
    else{
        global $language;
    }
    
    $photoid = (int)filtertext(strip_tags($_POST['photo_id']));
    $lastid = (int)filtertext(strip_tags($_POST['last_id']));
    $activeTab = filtertext(strip_tags($_POST['activeTab']));
    $video_key = (int)filtertext(strip_tags($_POST['video_key']));
    $audio_key = (int)filtertext(strip_tags($_POST['audio_key']));
    
    
    $result = array();
    if ($activeTab == 'video') {

        // Get Reference Video Items
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Video');
        $select->orderBy('id', 'ASC');
        $photoReferencesVideo = $select->execute()->fetchAll();

        //echo "<pre>";
        //print_r($photoReferencesVideo);
        //exit;
        $videoTotalCount = '';
        if (count($photoReferencesVideo) > 0) {
            $videoWithItems = explode(';', $photoReferencesVideo[0]->section_format);
            // seperate items
            for ($i = 0; $i < count($videoWithItems); $i++) {
                if ($videoWithItems[$i] != '')
                    $videoWithIdArray[] = explode(':', $videoWithItems[$i]);
            }
            // seperate video ids
            /* for ($i = 0; $i < count($videoWithCategory); $i++) {
                $videoWithIdArray[] = explode(':', $videoWithCategory[$i][1]);
            } */
            // Murger Video ids
            $videoWithId = '';
            if (count($videoWithIdArray) > 1) {
                $count = count($videoWithIdArray) - 1;
                for ($i = 0; $i < count($videoWithIdArray); $i++) {
                    if ($count == $i) {
                        $videoWithId .= $videoWithIdArray[$i][1];
                    } else {
                        $videoWithId .= $videoWithIdArray[$i][1] . ',';
                    }
                }
            } else {
                $videoWithId = $videoWithIdArray[0][1];
            }
            $totalCount = '0';
            // Sort Video ids
            if ($videoWithId != '') {
                $videoId = explode(',', $videoWithId); // asort(explode(',', $photoWithId));
                $videoTotalCount = count($videoId);
                sort($videoId);
                /* $videoWithId = implode(',', array_unique($videoId)); */
				$videoWithId = trim(implode(',', array_unique($videoId)),",");

                // Get reference photo from database
                $select = db_select('video_master', 'vm');
                $select->join('video_detail', 'vd', 'vd.video_id = vm.video_id');
                $select->fields('vm', array('video_id', 'category_id', 'video_filename', 'video_image', 'video_publish_status', 'video_approve_status'));
                $select->fields('vd', array('video_detail_id', 'videographer_name', 'language_code', 'video_title', 'video_description', 'video_duration'));
                $select->condition('video_publish_status', 'Y');
                $select->condition('video_approve_status', 'Y');
                $select->condition('language_code', $language->language);
                $select->where(' vm.video_id IN (' . $videoWithId . ')');

                $select->where('vm.video_id > ' . $lastid . '');

                $select->orderBy('video_id', 'ASC');
                $select = $select->extend('PagerDefault')->limit(3);
                $photoReferencesVideo = $select->execute()->fetchAll();

                $totalCount = count($photoReferencesVideo);
            }

            $result['totalResultCount'] = $totalCount;
            $result['photoReferencesVideo'] = $photoReferencesVideo;




            $output = theme('ghp_video_reference_loadmore', array(
                'ReferencesVideo' => $result['photoReferencesVideo'],
                'videoCount' => $result['videoTotalCount'],
                'activeTab' => $activeTab,
                'video_key' => $video_key
                
                    )
            );

            print_r($output);
            exit;
        }
    } else if ($activeTab == 'cartoon') {
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Cartoon');
        $select->orderBy('id', 'ASC');
        $photoReferencesCartoon = $select->execute()->fetchAll();

        $cartoonTotalCount = '';
        if (count($photoReferencesCartoon) > 0) {
            $cartoonWithItems = $photoReferencesCartoon[0]->section_format;

            // Sort cartoon ids
            if ($cartoonWithItems != '') {
                $cartoonId = explode(',', $cartoonWithItems); // asort(explode(',', $photoWithId));                
                sort($cartoonId);
                /* $cartoonWithId = implode(',', array_unique($cartoonId)); */
				$cartoonWithId = trim(implode(',', array_unique($cartoonId)),",");
				
                // Get reference cartoon from database
                $select = db_select('cartoon_master', 'cm');
                $select->join('cartoon_detail', 'cd', 'cd.cartoon_id = cm.cartoon_id');
                $select->fields('cm', array('cartoon_id', 'category_id', 'cartoon_filename', 'cartoon_flipimage', 'cartoon_publish_status', 'cartoon_approve_status'));
                $select->fields('cd', array('cartoon_detail_id', 'language_code', 'cartoon_title', 'cartoon_description'));
                $select->condition('cartoon_publish_status', 'Y');
                $select->condition('cartoon_approve_status', 'Y');
                $select->condition('language_code', $language->language);
                $select->where(' cm.cartoon_id IN (' . $cartoonWithId . ')');
                $select->where('cm.cartoon_id > ' . $lastid . '');
                $select->orderBy('cartoon_id', 'ASC');
                $select = $select->extend('PagerDefault')->limit(3);
                $photoReferencesCartoon = $select->execute()->fetchAll();
                $cartoonTotalCount = count($photoReferencesCartoon);
            }
            $result['cartoonTotalCount'] = $cartoonTotalCount;
            $result['photoReferencesCartoon'] = $photoReferencesCartoon;


            $output = theme('ghp_cartoon_reference_loadmore', array(
                'ReferencesCartoon' => $result['photoReferencesCartoon'],
                'cartoonCount' => $result['cartoonTotalCount'],
                'activeTab' => $activeTab
                    )
            );

            print_r($output);
            exit;
        }
    } else if ($activeTab == 'poster') {
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Poster');
        $select->orderBy('id', 'ASC');
        $photoReferencesPoster = $select->execute()->fetchAll();

        $posterTotalCount = '';
        if (count($photoReferencesPoster) > 0) {
            $posterWithItems = $photoReferencesPoster[0]->section_format;

            // Sort poster ids
            if ($posterWithItems != '') {
                $posterId = explode(',', $posterWithItems); // asort(explode(',', $photoWithId));                
                sort($posterId);
                /* $posterWithId = implode(',', array_unique($posterId)); */
				$posterWithId = trim(implode(',', array_unique($posterId)),",");

                // Get reference poster from database
                $select = db_select('poster_master', 'pm');
                $select->join('poster_detail', 'pd', 'pd.poster_id = pm.poster_id');
                $select->fields('pm', array('poster_id', 'category_id', 'poster_filename', 'poster_publish_status', 'poster_approve_status'));
                $select->fields('pd', array('poster_detail_id', 'language_code', 'poster_title', 'poster_description'));
                $select->condition('poster_publish_status', 'Y');
                $select->condition('poster_approve_status', 'Y');
                $select->condition('language_code', $language->language);
                $select->where(' pm.poster_id IN (' . $posterWithId . ')');
                $select->where('pm.poster_id > ' . $lastid . '');
                $select->orderBy('poster_id', 'ASC');
                $select = $select->extend('PagerDefault')->limit(3);
                $photoReferencesPoster = $select->execute()->fetchAll();
                $posterTotalCount = count($photoReferencesPoster);
            }
            $result['totalPosterCount'] = $posterTotalCount;
            $result['photoReferencesPoster'] = $photoReferencesPoster;

            $output = theme('ghp_poster_reference_loadmore', array(
                'ReferencesPoster' => $result['photoReferencesPoster'],
                'posterCount' => $result['totalPosterCount'],
                'activeTab' => $activeTab
                    )
            );

            print_r($output);
            exit;
        }
    } else if ($activeTab == 'stamp') {
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Stamp');
        $select->orderBy('id', 'ASC');
        $photoReferencesStamp = $select->execute()->fetchAll();

        $stampTotalCount = '';
        if (count($photoReferencesStamp) > 0) {
            $stampWithItems = $photoReferencesStamp[0]->section_format;

            // Sort poster ids
            if ($stampWithItems != '') {
                $stampId = explode(',', $stampWithItems); // asort(explode(',', $photoWithId));            
                sort($stampId);
                /* $stampWithId = implode(',', array_unique($stampId)); */
				$stampWithId = trim(implode(',', array_unique($stampId)),",");

                // Get reference poster from database
                $select = db_select('stamp_master', 'sm');
                $select->join('stamp_detail', 'sd', 'sd.stamp_id = sm.stamp_id');
                $select->join('category', 'c', 'sm.category_id  = c.category_id');
                $select->fields('sm', array('stamp_id', 'category_id', 'stamp_filename', 'stamp_publish_status', 'stamp_approve_status'));
                $select->fields('sd', array('stamp_detail_id', 'language_code', 'stamp_title', 'stamp_description'));
                $select->fields('c', array('slug'));
                $select->condition('stamp_publish_status', 'Y');
                $select->condition('stamp_approve_status', 'Y');
                $select->condition('language_code', $language->language);
                $select->where(' sm.stamp_id IN (' . $stampWithId . ')');
                $select->where(' sm.stamp_id > ' . $lastid . '');
                $select->orderBy('stamp_id', 'ASC');
                $selectTotal = $select;
                $select = $select->extend('PagerDefault')->limit(4);
                $photoReferencesStamp = $select->execute()->fetchAll();

                $stampTotalCount = count($photoReferencesStamp);
            }
            $result['stampTotalCount'] = $stampTotalCount;
            $result['photoReferencesStamp'] = $photoReferencesStamp;

            $output = theme('ghp_stamp_reference_loadmore', array(
                'ReferencesStamp' => $result['photoReferencesStamp'],
                'stampCount' => $result['stampTotalCount'],
                'activeTab' => $activeTab
                    )
            );

            print_r($output);
            exit;
        }
    } else if ($activeTab == 'audio') {
        // 
        // Get Reference Audio Items
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Audio');
        $select->orderBy('id', 'ASC');
        $photoReferencesAudio = $select->execute()->fetchAll();

        $audioTotalCount = '';
        if (count($photoReferencesAudio) > 0) {
            $audioWithItems = explode(';', $photoReferencesAudio[0]->section_format);
            // seperate items
            for ($i = 0; $i < count($audioWithItems); $i++) {
                if ($audioWithItems[$i] != '')
                    $audioWithIdArray[] = explode(':', $audioWithItems[$i]);
            }
            // seperate audio ids
            /* for ($i = 0; $i < count($audioWithCategory); $i++) {
                $audioWithIdArray[] = explode(':', $audioWithCategory[$i][1]);
            } */
            // Murger audio ids
            $audioWithId = '';
            if (count($audioWithIdArray) > 1) {
                $count = count($audioWithIdArray) - 1;
                for ($i = 0; $i < count($audioWithIdArray); $i++) {
                    if ($count == $i) {
                        $audioWithId .= $audioWithIdArray[$i][1];
                    } else {
                        $audioWithId .= $audioWithIdArray[$i][1] . ',';
                    }
                }
            } else {
                $audioWithId = $audioWithIdArray[0][1];
            }

            // Sort Audio ids
            if ($audioWithId != '') {
                $audioId = explode(',', $audioWithId); // asort(explode(',', $photoWithId));            
                sort($audioId);
                /* $audioWithId = implode(',', array_unique($audioId)); */
				$audioWithId = trim(implode(',', array_unique($audioId)),",");

                // Get reference audio from database
                $select = db_select('audio_master', 'am');
                $select->join('audio_detail', 'ad', 'ad.audio_id = am.audio_id');
                $select->fields('am', array('audio_id', 'category_id', 'audio_filename', 'audio_date', 'audio_publish_status', 'audio_approve_status'));
                $select->fields('ad', array('audio_detail_id', 'language_code', 'audio_title', 'audio_description', 'audio_duration', 'audio_place', 'ActualVolID', 'VolumePageRange'));
                $select->addExpression("DATE_FORMAT(am.audio_date,'%d/%m/%Y')", 'audio_date');
                $select->condition('audio_publish_status', 'Y');
                $select->condition('audio_approve_status', 'Y');
                $select->condition('language_code', $language->language);
                $select->where(' am.audio_id IN (' . $audioWithId . ')');
                $select->where(' am.audio_id > ' . $lastid . '');
                $select->orderBy('audio_id', 'ASC');
                //echo $select->__toString(); 
                $selectTotal = $select;
                $select = $select->extend('PagerDefault')->limit(3);
                $photoReferencesAudio = $select->execute()->fetchAll();
                $audioTotalCount = count($photoReferencesAudio);
            }
            $result['audioTotalCount'] = $audioTotalCount;
            $result['photoReferencesAudio'] = $photoReferencesAudio;

            $output = theme('ghp_audio_reference_loadmore', array(
                'ReferencesAudio' => $result['photoReferencesAudio'],
                'audioCount' => $result['audioTotalCount'],
                'activeTab' => $activeTab,
				'audio_key' => $audio_key
                    )
            );

            print_r($output);
            exit;
        }
    } else if ($activeTab == 'cwmg') {

        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Volume');
        $select->orderBy('id', 'ASC');
        $photoReferencesCWMG = $select->execute()->fetchAll();

        $cwmgTotalCount = $cwmgSections = '';
        if (count($photoReferencesCWMG) > 0) {

            $cwmgSections = $photoReferencesCWMG[0]->section_format;
            $cwmgWithItems = explode(';', $photoReferencesCWMG[0]->section_format);
            // seperate items
            for ($i = 0; $i < count($cwmgWithItems); $i++) {
                if ($cwmgWithItems[$i] != '')
                    $cwmgWithIdArray[] = explode(':', $cwmgWithItems[$i]);
            }
            // seperate audio ids
            /* for ($i = 0; $i < count($cwmgWithCategory); $i++) {
                //$cwmgWithIdArray[] = explode(':', $cwmgWithCategory[$i][0]);
                $cwmgWithIdArray[] =  $cwmgWithCategory[$i][0];
				
            } */
            // Murger audio ids
            $cwmgWithId = '';
            if (count($cwmgWithIdArray) > 1) {
                $count = count($cwmgWithIdArray) - 1;
                for ($i = 0; $i < count($cwmgWithIdArray); $i++) {
                    if ($count == $i) {
                        $cwmgWithId .= $cwmgWithIdArray[$i][0];
                    } else {
                        $cwmgWithId .= $cwmgWithIdArray[$i][0] . ',';
                    }
                }
            } else {
                $cwmgWithId = $cwmgWithIdArray[0][0];
            }

            // Sort Audio ids
            if ($cwmgWithId != '') {
                $cwmgId = explode(',', $cwmgWithId); // asort(explode(',', $photoWithId));            
                sort($cwmgId);
                /* $cwmgWithId = implode(',', array_unique($cwmgId)); */
				$cwmgWithId = trim(implode(',', array_unique($cwmgId)),",");

                $select_cwmg_language = db_select('cwmg_language', 'cwmgl');
                $select_cwmg_language->fields('cwmgl', array('language_id'));
                $select_cwmg_language->condition('language_code', $language->language);
                $cwmg_language_data_obj = $select_cwmg_language->execute();
                $cwmg_language_count = $cwmg_language_data_obj->rowCount();
                $cwmg_language_data = $cwmg_language_data_obj->fetchAssoc();

                if( $cwmg_language_count>0 && !empty($cwmg_language_data['language_id']) )
                {
                    // Get reference audio from database
                    $select = db_select('volume_master', 'vm');
                    $select->fields('vm', array('ActualVol', 'LanguageID', 'VolumeName', 'ActualDisplayPages', 'NumberOfPages'));
                    $select->condition('volume_publish_status', 'Y');
                    $select->condition('volume_approve_status', 'Y');
                    $select->condition('LanguageID', $cwmg_language_data['language_id']);
                    $select->where(' vm.ActualVol IN (' . $cwmgWithId . ')');
                    $select->where(' vm.ActualVol > ' . $lastid . '');
                    $select->orderBy('ActualVol', 'ASC');
                    //echo $select->__toString(); 
                    $selectTotal = $select;
                    $cwmgTotalCount = $selectTotal->execute()->rowCount();
                    $select = $select->extend('PagerDefault')->limit(3);
                    $photoReferencesCWMG = $select->execute()->fetchAll();
                    $cwmgTotalCount = count($photoReferencesCWMG);
                    $result['photoReferencesCWMG'] = $photoReferencesCWMG;
                    $result['cwmgTotalCount'] = $cwmgTotalCount;
                }


                
            }

            $output = theme('ghp_cwmg_reference_loadmore', array(
                'ReferencesCWMG' => $result['photoReferencesCWMG'],
                'cwmgCount' => $result['cwmgTotalCount'],
                'activeTab' => $activeTab,
                'cwmgSections' => $cwmgSections
                    )
            );

            print_r($output);
            exit;
        }
    } else if ($activeTab == 'tribute') {

        //Get Reference Tribute Items
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Tribute');
        $select->orderBy('id', 'ASC');
        $photoReferencesTribute = $select->execute()->fetchAll();

        $tributeTotalCount = '';
        if (count($photoReferencesTribute) > 0) {
            $tributeWithItems = $photoReferencesTribute[0]->section_format;

            // Sort tributes ids
            if ($tributeWithItems != '') {
                $tributeId = explode(',', $tributeWithItems);
                sort($tributeId);
                /* $tributeWithId = implode(',', array_unique($tributeId)); */
				$tributeWithId = trim(implode(',', array_unique($tributeId)),",");

                // Get reference tributes from database
                $select = db_select('tribute_detail', 'td');
                $select->join('tribute_master', 'tm', 'tm.tributeid  = td.tributeid');
                $select->join('tributeperson_detail', 'tpd', 'tpd. personid  = tm.tributeby');
                $select->join('tributeperson_master', 'tpm', 'tpm.personid  = tpd.personid');
                $select->fields('tpm', array('personphoto'));
                $select->fields('tpd', array('personid', 'firstname', 'lastname', 'personprofile'));
                $select->fields('td', array('id', 'tributeid', 'title', 'tributetext', 'tributeplace'));
                $select->where('tpm.personid IN (' . $tributeWithId . ')');
                $select->where(' tpm.personid > ' . $lastid . '');
                $select->condition('td.languagecode', $language->language);
                $select->condition('tpd.languagecode', $language->language);
                $select->condition('td.tributepublishstatus', "Y");
                $select->condition('td.tributeapprovedstatus', "Y");
                $select->orderBy('personid', 'ASC');
                $selectTotal = $select;
                $tributeTotalCount = $selectTotal->execute()->rowCount();
                $select = $select->extend('PagerDefault')->limit(3);
                $photoReferencesTribute = $select->execute()->fetchAll();
                //echo "<pre>";
                //print_r($photoReferencesTribute);
                //exit;
            }

            $result['tributeTotalCount'] = $tributeTotalCount;
            $result['photoReferencesTribute'] = $photoReferencesTribute;

            $output = theme('ghp_tribute_reference_loadmore', array(
                'ReferencesTribute' => $result['photoReferencesTribute'],
                'tributeCount' => $result['tributeTotalCount'],
                'activeTab' => $activeTab
                    )
            );

            print_r($output);
            exit;
        }
        // END FOR RELATED TRIBUTE CODE
    } else if ($activeTab == 'variorum') {

        // Get Related variorum from database
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Variorum');
        $select->orderBy('id', 'ASC');
        $photoReferencesVariorum = $select->execute()->fetchAll();

        $variorumTotalCount = '';
        if (count($photoReferencesVariorum) > 0) {
            $variorumWithItems = $photoReferencesVariorum[0]->section_format;

            // Sort poster ids
            if ($variorumWithItems != '') {
                $variorumId = explode(',', $variorumWithItems); // asort(explode(',', $photoWithId));            
                sort($variorumId);

                /* $variorumWithId = implode(',', array_unique($variorumId)); */
				$variorumWithId = trim(implode(',', array_unique($variorumId)),",");

                // Get reference of veroriam table
                $select = db_select('veroriam_master', 'vm');
                $select->join('veroriam_document_type', 'vdt', 'vm.document_type = vdt.id');
                $select->join('veroriam_language', 'vl', 'vm.language = vl.id');
                $select->fields('vm', array('id', 'serial_number', 'serial_number_int', 'treatment', 'vdate', 'document_type', 'document_type_text', 'document_copy', 'no_enclosures', 'from_name', 'to_name', 'from_place', 'to_place', 'missing_letters', 'language', 'no_page', 'classification', 'subject', 'status', 'created'));
                $select->fields('vdt', array('title'));
                $select->fields('vl', array('title'));
                $select->where(' vm.id IN (' . $variorumWithId . ')');
                $select->where(' vm.id  > ' . $lastid . '');
                $select->condition('vl.tag', $language->language);
                $select->orderBy('id', 'ASC');
                $selectTotal = $select;
                $variorumTotalCount = $selectTotal->execute()->rowCount();
                $result['variorumTotalCount'] = $variorumTotalCount;
                $select = $select->extend('PagerDefault')->element(9)->limit(3);
                $photoReferencesVariorum = $select->execute()->fetchAll();
                $result['photoReferencesVariorum'] = $photoReferencesVariorum;
            }
            $output = theme('ghp_variorum_reference_loadmore', array(
                'ReferencesVariorum' => $result['photoReferencesVariorum'],
                'variorumCount' => $result['variorumTotalCount'],
                'activeTab' => $activeTab
                    )
            );

            print_r($output);
            exit;
        }
        // End of related variorum code !!
    //
        
    }
    else if ($activeTab == 'book') {

        //Get Reference book Items
        $select = db_select('photo_master', 'pm');
        $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
        $select->fields('pm', array('photo_date', 'created_by'));
        $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
        $select->condition('ref_id', $photoid);
        $select->condition('module_name', 'Photo');
        $select->condition('section_name', 'Book');
        $select->orderBy('id', 'ASC');
        $photoReferencesBook = $select->execute()->fetchAll();

        $bookTotalCount = $booksSections = '';
		
		
		
        if (count($photoReferencesBook) > 0) {
            //$bookWithItems = $photoReferencesBook[0]->section_format;
			 $booksSections = $photoReferencesBook[0]->section_format;
            $booksWithItems = explode(';', $photoReferencesBook[0]->section_format);
            // seperate items
            for ($i = 0; $i < count($booksWithItems); $i++) {
                if ($booksWithItems[$i] != '')
                    $booksWithIdArray[] = explode(':', $booksWithItems[$i]);
            }
            
            // Murger audio ids
            $bookWithItems = '';
            if (count($booksWithIdArray) > 1) {
                $count = count($booksWithIdArray) - 1;
                for ($i = 0; $i < count($booksWithIdArray); $i++) {
                    if ($count == $i) {
                        $bookWithItems .= $booksWithIdArray[$i][0];
                    } else {
                        $bookWithItems .= $booksWithIdArray[$i][0] . ',';
                    }
                }
            } else {
                $bookWithItems = $booksWithIdArray[0][0];
            }
			
            // Sort book ids
            if ($bookWithItems != '') {
                $bookId = explode(',', $bookWithItems);
                sort($bookId);
                /* $bookWithId = implode(',', array_unique($bookId)); */
				$bookWithId = trim(implode(',', array_unique($bookId)),",");

                // Get reference book from database
                
            $select = db_select('books', 'b');
            $select->join('books_language', 'bl', 'b.LanguageID = bl.language_id');
            $select->fields('bl', array('language_id','language_code'));
            $select->fields('b', array('BookId', 'ActualBook', 'Title', 'Author', 'Latitude', 'Longitude', 'PlaceName', 'QuotedIn', 'PublishDate', 'StartDate', 'EndDate','BookImg','book_order_number','front_cover','copyright','table_content','first_page','index_number','back_cover'));
            $select->condition('book_publish_status', 'Y');
            $select->condition('book_approve_status', 'Y');
            $select->where(' b.BookId IN (' . $bookWithId . ')');
            $select->where(' b.BookId  > ' . $lastid . '');
            $select->condition('bl.language_code', $language->language);
            $select->orderBy('BookId', 'ASC');
            $selectTotal = $select;

            $bookTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->limit(3);
            $photoReferencesBook = $select->execute()->fetchAll();
                
            }

            $result['bookTotalCount'] = $bookTotalCount;
            $result['photoReferencesBook'] = $photoReferencesBook;

            $output = theme('ghp_book_reference_loadmore', array(
                'ReferencesBook' => $result['photoReferencesBook'],
                'bookCount' => $result['bookTotalCount'],
                'activeTab' => $activeTab,
				'booksSections' => $booksSections
                    )
            );

            print_r($output);
            exit;
        }
    }
    else if ($activeTab == 'journalbygandhi') {

        //Get Reference journalbygandhi Items
        $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'JournalByGandhiji');
    $select->orderBy('id', 'ASC');
    $photoReferencesJournalgandhi = $select->execute()->fetchAll();

        $journalgandhiTotalCount = '';
        if (count($photoReferencesJournalgandhi) > 0) {
            $journalgandhiWithItems = $photoReferencesJournalgandhi[0]->section_format;

            // Sort book ids
            if ($journalgandhiWithItems != '') {
                // $journalgandhiId = explode(',', $journalgandhiWithItems);
                // sort($bookId);
                // $journalgandhiWithId = implode(',', array_unique($journalgandhiId));
                $journalgandhiArray = explode(';', $journalgandhiWithItems);

                $journalIDsArray = array();
                for ($i = 0; $i < count($journalgandhiArray); $i++) {
                    $journalVolumeIssuePagesData = explode(':', $journalgandhiArray[$i]);
                    $journalVolumeIssueIDsArray = explode('-', $journalVolumeIssuePagesData[0]);
                    $journalIDsArray[] = $journalVolumeIssueIDsArray[0];
                }
                sort($journalIDsArray);

                $journalgandhiWithId = trim(implode(',', array_unique($journalIDsArray)),",");

                // Get reference book from database
                
             $select = db_select('journal_master', 'jm');
			$select->fields( 'jm', array('journal_id', 'journal_cover_image') );
			$select->fields( 'jd', array('journal_title') );
			$select->leftJoin('journal_detail', 'jd', 'jm.journal_id = jd.journal_id');
			// $select->condition('language_code', $language->language);
			$select->condition('journal_publish_status', 'Y');
			$select->condition('journal_approve_status', 'Y');
			$select->condition('journal_parent_id', '7');
			$select->where(' jm.journal_id IN (' . $journalgandhiWithId . ')');
			 $select->where(' jm.journal_id  > ' . $lastid . '');
            $select->orderBy('journal_id', 'ASC');
            $selectTotal = $select;

            $journalgandhiTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->limit(4);
            $photoReferencesJournalgandhi = $select->execute()->fetchAll();
            }

            $result['journalgandhiTotalCount'] = $journalgandhiTotalCount;
            $result['photoReferencesJournalgandhi'] = $photoReferencesJournalgandhi;

            $result['journalgandhi_section_format'] = $journalgandhiWithItems;
			
            $output = theme('ghp_journal_by_gandhi_reference_loadmore', array(
                'ReferencesJournalgandhi' => $result['photoReferencesJournalgandhi'],
                'journalbygandhiCount' => $result['journalgandhiTotalCount'],
                'journalgandhi_section_format' => $result['journalgandhi_section_format'],
                'activeTab' => $activeTab
                    )
            );
            
            

            print_r($output);
            exit;
        }
    }
    else if ($activeTab == 'journalbyother') {

        //Get Reference journalbyother Items
      $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'JournalByOthers');
    $select->orderBy('id', 'ASC');
    $photoReferencesJournalother = $select->execute()->fetchAll();

    $journalotherTotalCount = '';
    if (count($photoReferencesJournalother) > 0) {
        $journalotherWithItems = $photoReferencesJournalother[0]->section_format;

        // Sort poster ids
        if ($journalotherWithItems != '') {
            // $journalotherId = explode(',', $journalotherWithItems); // asort(explode(',', $photoWithId));            
            // sort($journalotherId);

            // $journalotherWithId = implode(',', array_unique($journalotherId));

            $journalotherArray = explode(';', $journalotherWithItems);

            $journalIDsArray = array();
            for ($i = 0; $i < count($journalotherArray); $i++) {
                $journalVolumeIssuePagesData = explode(':', $journalotherArray[$i]);
                $journalVolumeIssueIDsArray = explode('-', $journalVolumeIssuePagesData[0]);
                $journalIDsArray[] = $journalVolumeIssueIDsArray[0];
            }
            sort($journalIDsArray);

            $journalotherWithId = trim(implode(',', array_unique($journalIDsArray)),",");

            // Get reference of journal by gandhi table
            $select = db_select('journal_master', 'jm');
			$select->fields( 'jm', array('journal_id', 'journal_cover_image') );
			$select->fields( 'jd', array('journal_title') );
			$select->leftJoin('journal_detail', 'jd', 'jm.journal_id = jd.journal_id');
			// $select->condition('language_code', $language->language);
			$select->condition('journal_publish_status', 'Y');
			$select->condition('journal_approve_status', 'Y');
			$select->condition('journal_parent_id', '8');
			$select->where(' jm.journal_id IN (' . $journalotherWithId . ')');
			$select->where(' jm.journal_id  > ' . $lastid . '');
            $select->orderBy('journal_id', 'ASC');
            $selectTotal = $select;

            $journalotherTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->limit(4);
                $photoReferencesJournalother = $select->execute()->fetchAll();
                /*echo "<pre>";
                print_r($photoReferencesJournalother);
                exit;*/
               
            }

            $result['journalotherTotalCount'] = $journalotherTotalCount;
            $result['photoReferencesJournalother'] = $photoReferencesJournalother;

            $result['journalother_section_format'] = $journalotherWithItems;

            $output = theme('ghp_journal_by_other_reference_loadmore', array(
                'ReferencesJournalother' => $result['photoReferencesJournalother'],
                'journalbyotherCount' => $result['journalotherTotalCount'],
                'journalother_section_format' => $result['journalother_section_format'],
                'activeTab' => $activeTab
                    )
            );

            print_r($output);
            exit;
        }
    }else if ($activeTab == 'othersection') {
        
    $active_cat =  $_POST['active_cat'];  
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'FundamentalWork');
    $select->orderBy('id', 'ASC');
    $photoReferencesbooksections = $select->execute()->fetchAll();

    
    $booksectionsTotalCount = '';
    if (count($photoReferencesbooksections) > 0) {
        $bookWithItems = explode(';', $photoReferencesbooksections[0]->section_format);
        
        
        // seperate items
        for ($i = 0; $i < count($bookWithItems); $i++) {
            if ($bookWithItems[$i] != '')
                $bookWithCategory[] = explode(':', $bookWithItems[$i]);
        }
        $listbooksresult = array();
        $booksectionTotalCount = 0;
        // seperate audio ids
        for ($i = 0; $i < count($bookWithCategory); $i++) {
            
            if($bookWithCategory[$i][0] == $active_cat )
            {   
            
                $term = taxonomy_term_load($bookWithCategory[$i][0]);
                $Cat_name = $term->name;
                //$bookWithIdArray = explode(':', $bookWithCategory[$i][1]);
                $bookWithIdArray = $bookWithCategory[$i][1];

                $bookWithIdData = explode(',', $bookWithIdArray);

                $bookIds = array();
                foreach($bookWithIdData as $bookWithId)
                {
                    $bookIdArray = explode('-', $bookWithId);
                    $bookIds[] = $bookIdArray[0];
                }

                sort($bookIds);

                $bookWithIdArray = trim(implode(',', array_unique($bookIds)),",");					
					
                if(!empty($bookWithIdArray)){


                    $select = db_select('booksection', 'b');
                    $select->fields('b', array('FundamentalworkId', 'ActualFundamentalwork', 'FundamentalworkImg', 'Title'));
                    $select->fields('bd', array('LanguageID', 'author'));
                    $select->fields('bl', array('language_code'));
                    $select->fields('bc', array('BookCategoryId'));
                    $select->Join('booksection_detail', 'bd', 'b.ActualFundamentalwork = bd.ActualFundamentalworkID');
                    $select->Join('booksection_language', 'bl', 'b.LanguageID = bl.language_id');
                    $select->leftJoin('booksection_categories', 'bc', 'b.FundamentalworkId = bc.fundamental_book_id');
                    $select->condition('bc.BookCategoryId', $bookWithCategory[$i][0]);
                    $select->where('b.LanguageID = bd.LanguageID');
                    $select->where('b.FundamentalworkId in ('.$bookWithIdArray.')');
                    $select->where('b.FundamentalworkId   > ' . $lastid . '');
                    //$select->condition('bl.language_code', $language->language);
                    $select->groupby('b.FundamentalworkId');
                    //$test = $select->execute()->fetchAll();


                    $selectTotal = $select;
                    $booksectionTotalCount = $booksectionTotalCount + $selectTotal->execute()->rowCount();
                    $listbooksresult[$Cat_name.'-'.$bookWithCategory[$i][0]]['sectionTotalcount'] = $selectTotal->execute()->rowCount();
                    $select = $select->extend('PagerDefault')->element(14)->limit(3);
                    $listbooksresult[$Cat_name.'-'.$bookWithCategory[$i][0]]['sectionItemListing'] = $select->execute()->fetchAll();
              
                }
            }
        }
        //print_r($listbooksresult); 
        
        $result['booksectionTotalCount'] = $booksectionTotalCount;
        $result['photoReferencesBooksection'] = $listbooksresult;

        $result['booksection_section_format'] = $photoReferencesbooksections[0]->section_format;
        
        $output = theme('ghp_othersection_reference_loadmore', array(
                'Referencesothersection' => $result['photoReferencesBooksection'],
                'othersectionCount' => $result['booksectionTotalCount'],
                'booksection_section_format' => $result['booksection_section_format'],
                'activeTab' => $activeTab,
                'active_cat'=>$active_cat,
                    )
            );
       

            print_r($output);
            exit;
     
    }
        
        
    }else {
        $result['totalResultCount'] = 0;
        $result['result_display_data'] = '';
    }

    print_r($result);
    exit;
}

function photos_detail_page() {

    global $base_path;
    global $base_url;
    global $language;
    drupal_add_js(drupal_get_path('module', 'cwmg_language') . '/js/jquery_172.js');

    if (arg(2) == 'stamp') {
        drupal_add_css(drupal_get_path('module', 'ghp_stamps') . '/css/thickbox.css');
    } else {
        drupal_add_css(drupal_get_path('module', 'cwmg_language') . '/css/thickbox.css');
    }

    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/thickbox.js');
    drupal_add_js(drupal_get_path('module', 'gandhi_heritage_site') . '/js/base_enc.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/htmlentities.js');
    drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/jquery.mCustomScrollbar.css');

    drupal_add_js(drupal_get_path('module', 'ghp_audios') . '/js/carousel.min.js');

    drupal_add_css(drupal_get_path('module', 'ghp_gallery') . '/css/jshowcase.css');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.livequery.js');

    // START FOR JPLAYER
    drupal_add_js(drupal_get_path('module', 'ghp_videos') . '/jplayer/js/jquery.jplayer.min.js');
    drupal_add_js(drupal_get_path('module', 'ghp_videos') . '/jplayer/js/jplayer.playlist.min.js');
    drupal_add_css(drupal_get_path('module', 'ghp_videos') . '/jplayer/skin/blue.monday/jplayer.blue.monday.css');
    // STOP FOR JPLAYER

    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.tablesorter.min.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.mCustomScrollbar.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photo_detail.js');

    $photoid = filtertext(strip_tags(base64_decode(arg(1))));

    // Get Reference Video Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Video');
    $select->orderBy('id', 'ASC');
    $photoReferencesVideo = $select->execute()->fetchAll();
    //echo "<pre>";
    //print_r($photoReferencesVideo);
    //exit;

    $videoTotalCount = '';
    if (count($photoReferencesVideo) > 0) {
        $videoWithItems = explode(';', $photoReferencesVideo[0]->section_format);

        // seperate items
        for ($i = 0; $i < count($videoWithItems); $i++) {
            if ($videoWithItems[$i] != '')
                $videoWithIdArray[] = explode(':', $videoWithItems[$i]);
        }
        // seperate video ids
        /* for ($i = 0; $i < count($videoWithCategory); $i++) {
            $videoWithIdArray[] = explode(':', $videoWithCategory[$i][1]);
        } */
        // Murger Video ids
        $videoWithId = '';
        if (count($videoWithIdArray) > 1) {
            $count = count($videoWithIdArray) - 1;
            for ($i = 0; $i < count($videoWithIdArray); $i++) {
                if ($count == $i) {
                    $videoWithId .= $videoWithIdArray[$i][1];
                } else {
                    $videoWithId .= $videoWithIdArray[$i][1] . ',';
                }
            }
        } else {
            $videoWithId = $videoWithIdArray[0][1];
        }
        // Sort Video ids
        if ($videoWithId != '') {
            $videoId = explode(',', $videoWithId); // asort(explode(',', $photoWithId));            
            sort($videoId);
            /* $videoWithId = implode(',', array_unique($videoId)); */
 			$videoWithId = trim(implode(',', array_unique($videoId)),",");
		
            // Get reference video from database
            $select = db_select('video_master', 'vm');
            $select->join('video_detail', 'vd', 'vd.video_id = vm.video_id');
            $select->fields('vm', array('video_id', 'category_id', 'video_filename', 'video_image', 'video_publish_status', 'video_approve_status'));
            $select->fields('vd', array('video_detail_id', 'language_code', 'videographer_name', 'video_title', 'video_description', 'video_duration'));
            $select->condition('video_publish_status', 'Y');
            $select->condition('video_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' vm.video_id IN (' . $videoWithId . ')');
            $select->orderBy('video_id', 'ASC');
            $selectTotal = $select;
            $videoTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->element(3)->limit(3);
            $photoReferencesVideo = $select->execute()->fetchAll();
        }
    }
    // End of related Video item code
    // 
    // Get Related Cartoons from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Cartoon');
    $select->orderBy('id', 'ASC');
    $photoReferencesCartoon = $select->execute()->fetchAll();

    $cartoonTotalCount = '';
    if (count($photoReferencesCartoon) > 0) {
        $cartoonWithItems = $photoReferencesCartoon[0]->section_format;

        // Sort cartoon ids
        if ($cartoonWithItems != '') {
            $cartoonId = explode(',', $cartoonWithItems); // asort(explode(',', $photoWithId));            
            sort($cartoonId);
            /* $cartoonWithId = implode(',', array_unique($cartoonId)); */
 			$cartoonWithId = trim(implode(',', array_unique($cartoonId)),",");

            // Get reference cartoon from database
            $select = db_select('cartoon_master', 'cm');
            $select->join('cartoon_detail', 'cd', 'cd.cartoon_id = cm.cartoon_id');
            $select->fields('cm', array('cartoon_id', 'category_id', 'cartoon_filename', 'cartoon_flipimage', 'cartoon_publish_status', 'cartoon_approve_status'));
            $select->fields('cd', array('cartoon_detail_id', 'language_code', 'cartoon_title', 'cartoon_description'));
            $select->condition('cartoon_publish_status', 'Y');
            $select->condition('cartoon_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' cm.cartoon_id IN (' . $cartoonWithId . ')');
            $select->orderBy('cartoon_id', 'ASC');
            $selectTotal = $select;
            $cartoonTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->element(4)->limit(3);
            $photoReferencesCartoon = $select->execute()->fetchAll();

            //echo "<pre>";
            //print_r($photoReferencesCartoon);
            //exit;
        }
    }
    // End of related Cartoon code !!
    // 
    // Get Reference Audio Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Audio');
    $select->orderBy('id', 'ASC');
    $photoReferencesAudio = $select->execute()->fetchAll();
    //echo "<pre>";
    //print_r($photoReferencesAudio);
    //exit;

    $audioTotalCount = '';
    if (count($photoReferencesAudio) > 0) {
        $audioWithItems = explode(';', $photoReferencesAudio[0]->section_format);
        // seperate items
        for ($i = 0; $i < count($audioWithItems); $i++) {
            if ($audioWithItems[$i] != '')
                $audioWithIdArray[] = explode(':', $audioWithItems[$i]);
        }
        // seperate audio ids
        /* for ($i = 0; $i < count($audioWithCategory); $i++) {
            $audioWithIdArray[] = explode(':', $audioWithCategory[$i][1]);
        } */
        // Murger audio ids
        $audioWithId = '';
        if (count($audioWithIdArray) > 1) {
            $count = count($audioWithIdArray) - 1;
            for ($i = 0; $i < count($audioWithIdArray); $i++) {
                if ($count == $i) {
                    $audioWithId .= $audioWithIdArray[$i][1];
                } else {
                    $audioWithId .= $audioWithIdArray[$i][1] . ',';
                }
            }
        } else {
            $audioWithId = $audioWithIdArray[0][1];
        }

        // Sort Audio ids
        if ($audioWithId != '') {
            $audioId = explode(',', $audioWithId); // asort(explode(',', $photoWithId));            
            sort($audioId);
            /* $audioWithId = implode(',', array_unique($audioId)); */
 			$audioWithId = trim(implode(',', array_unique($audioId)),",");

            // Get reference audio from database
            $select = db_select('audio_master', 'am');
            $select->join('audio_detail', 'ad', 'ad.audio_id = am.audio_id');
            $select->fields('am', array('audio_id', 'category_id', 'audio_filename', 'audio_date', 'audio_publish_status', 'audio_approve_status'));
            $select->fields('ad', array('audio_detail_id', 'language_code', 'audio_title', 'audio_description', 'audio_duration', 'audio_place', 'ActualVolID', 'VolumePageRange'));
            $select->addExpression("DATE_FORMAT(am.audio_date,'%d/%m/%Y')", 'audio_date');
            $select->condition('audio_publish_status', 'Y');
            $select->condition('audio_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' am.audio_id IN (' . $audioWithId . ')');
            $select->orderBy('audio_id', 'ASC');
            //echo $select->__toString(); 
            $selectTotal = $select;
            $audioTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->element(5)->limit(3);
            $photoReferencesAudio = $select->execute()->fetchAll();
        }
    }
    // End of related Audio item code    
    // 
    // Get Related Poster from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Poster');
    $select->orderBy('id', 'ASC');
    $photoReferencesPoster = $select->execute()->fetchAll();

    $posterTotalCount = '';
    if (count($photoReferencesPoster) > 0) {
        $posterWithItems = $photoReferencesPoster[0]->section_format;

        // Sort poster ids
        if ($posterWithItems != '') {
            $posterId = explode(',', $posterWithItems); // asort(explode(',', $photoWithId));            
            sort($posterId);
            /* $posterWithId = implode(',', array_unique($posterId)); */
 			$posterWithId = trim(implode(',', array_unique($posterId)),",");

            // Get reference poster from database
            $select = db_select('poster_master', 'pm');
            $select->join('poster_detail', 'pd', 'pd.poster_id = pm.poster_id');
            $select->fields('pm', array('poster_id', 'category_id', 'poster_filename', 'poster_publish_status', 'poster_approve_status'));
            $select->fields('pd', array('poster_detail_id', 'language_code', 'poster_title', 'poster_description'));
            $select->condition('poster_publish_status', 'Y');
            $select->condition('poster_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' pm.poster_id IN (' . $posterWithId . ')');
            $select->orderBy('poster_id', 'ASC');
            $selectTotal = $select;
            $posterTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->element(6)->limit(3);
            $photoReferencesPoster = $select->execute()->fetchAll();
        }
    }
    // End of related Poster code !!
    //
    // Get Related Stamp from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Stamp');
    $select->orderBy('id', 'ASC');
    $photoReferencesStamp = $select->execute()->fetchAll();

    $stampTotalCount = '';
    if (count($photoReferencesStamp) > 0) {
        $stampWithItems = $photoReferencesStamp[0]->section_format;

        // Sort poster ids
        if ($stampWithItems != '') {
            $stampId = explode(',', $stampWithItems); // asort(explode(',', $photoWithId));            
            sort($stampId);
            /* $stampWithId = implode(',', array_unique($stampId)); */
 			$stampWithId = trim(implode(',', array_unique($stampId)),",");

            // Get reference poster from database
            $select = db_select('stamp_master', 'sm');
            $select->join('stamp_detail', 'sd', 'sd.stamp_id = sm.stamp_id');
            $select->join('category', 'c', 'sm.category_id  = c.category_id');
            $select->fields('sm', array('stamp_id', 'category_id', 'stamp_filename', 'stamp_publish_status', 'stamp_approve_status'));
            $select->fields('sd', array('stamp_detail_id', 'language_code', 'stamp_title', 'stamp_description'));
            $select->fields('c', array('slug'));
            $select->condition('stamp_publish_status', 'Y');
            $select->condition('stamp_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' sm.stamp_id IN (' . $stampWithId . ')');
            $select->orderBy('stamp_id', 'ASC');
            $selectTotal = $select;
            $stampTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->element(7)->limit(4);
            $photoReferencesStamp = $select->execute()->fetchAll();
        }
    }
    // End of related Stamp code !!
    //
    // Get Reference CWMG Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Volume');
    $select->orderBy('id', 'ASC');
    $photoReferencesCWMG = $select->execute()->fetchAll();

    $cwmgTotalCount = $cwmgSections = '';
    if (count($photoReferencesCWMG) > 0) {
        $cwmgSections = $photoReferencesCWMG[0]->section_format;
        //echo $cwmgSections; exit; 
        $cwmgWithItems = explode(';', $photoReferencesCWMG[0]->section_format);
        // seperate items
        for ($i = 0; $i < count($cwmgWithItems); $i++) {
            if ($cwmgWithItems[$i] != '')
                $cwmgWithIdArray[] = explode(':', $cwmgWithItems[$i]);
        }
        // seperate audio ids
        /* for ($i = 0; $i < count($cwmgWithCategory); $i++) {
            //$cwmgWithIdArray[] = explode(':', $cwmgWithCategory[$i][0]);
            $cwmgWithIdArray[] =  $cwmgWithCategory[$i][0];
			
        } */
        // Murger audio ids
        $cwmgWithId = '';
        if (count($cwmgWithIdArray) > 1) {
            $count = count($cwmgWithIdArray) - 1;
            for ($i = 0; $i < count($cwmgWithIdArray); $i++) {
                if ($count == $i) {
                    $cwmgWithId .= $cwmgWithIdArray[$i][0];
                } else {
                    $cwmgWithId .= $cwmgWithIdArray[$i][0] . ',';
                }
            }
        } else {
            $cwmgWithId = $cwmgWithIdArray[0][0];
        }

        // Sort Audio ids
        if ($cwmgWithId != '') {
            $cwmgId = explode(',', $cwmgWithId); // asort(explode(',', $photoWithId));            
            sort($cwmgId);
            /* $cwmgWithId = implode(',', array_unique($cwmgId)); */
			$cwmgWithId = trim(implode(',', array_unique($cwmgId)),",");
            $select_cwmg_language = db_select('cwmg_language', 'cwmgl');
            $select_cwmg_language->fields('cwmgl', array('language_id'));
            $select_cwmg_language->condition('language_code', $language->language);
            $cwmg_language_data_obj = $select_cwmg_language->execute();
            $cwmg_language_count = $cwmg_language_data_obj->rowCount();
            $cwmg_language_data = $cwmg_language_data_obj->fetchAssoc();
            if( $cwmg_language_count>0 && !empty($cwmg_language_data['language_id']) )
            {
                // Get reference audio from database
                $select = db_select('volume_master', 'vm');
                $select->fields('vm', array('ActualVol', 'LanguageID', 'VolumeName', 'ActualDisplayPages', 'NumberOfPages'));
                $select->condition('volume_publish_status', 'Y');
				$select->condition('volume_approve_status', 'Y');
                $select->condition('LanguageID', $cwmg_language_data['language_id']);
                $select->where(' vm.ActualVol IN (' . $cwmgWithId . ')');
                $select->orderBy('ActualVol', 'ASC');
                //echo $select->__toString(); 
                $selectTotal = $select;
                $cwmgTotalCount = $selectTotal->execute()->rowCount();
                $select = $select->extend('PagerDefault')->element(8)->limit(3);
                $photoReferencesCWMG = $select->execute()->fetchAll();
            }
            
        }
    }
    // End of related Audio item code    
    //Get Reference Tribute Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Tribute');
    $select->orderBy('id', 'ASC');
    $photoReferencesTribute = $select->execute()->fetchAll();
    //echo "<pre>";
    //print_r($photoReferencesTribute);
    //exit;

    $tributeTotalCount = '';
    if (count($photoReferencesTribute) > 0) {
        $tributeWithItems = $photoReferencesTribute[0]->section_format;

        // Sort tributes ids
        if ($tributeWithItems != '') {
            $tributeId = explode(',', $tributeWithItems);
            sort($tributeId);
            /* $tributeWithId = implode(',', array_unique($tributeId)); */
			$tributeWithId = trim(implode(',', array_unique($tributeId)),",");

            // Get reference tributes from database
            $select = db_select('tribute_detail', 'td');
            $select->join('tribute_master', 'tm', 'tm.tributeid  = td.tributeid');
            $select->join('tributeperson_detail', 'tpd', 'tpd. personid  = tm.tributeby');
            $select->join('tributeperson_master', 'tpm', 'tpm.personid  = tpd.personid');
            $select->fields('tpm', array('personphoto'));
            $select->fields('tpd', array('personid', 'firstname', 'lastname', 'personprofile'));
            $select->fields('td', array('id', 'tributeid', 'title', 'tributetext', 'tributeplace'));
            $select->where('tpm.personid IN (' . $tributeWithId . ')');
            $select->condition('td.languagecode', $language->language);
            $select->condition('tpd.languagecode', $language->language);
            $select->condition('td.tributepublishstatus', "Y");
            $select->condition('td.tributeapprovedstatus', "Y");
            $select->orderBy('personid', 'ASC');
            $selectTotal = $select;
            $tributeTotalCount = $selectTotal->execute()->rowCount();
            $select = $select->extend('PagerDefault')->element(10)->limit(3);
            $photoReferencesTribute = $select->execute()->fetchAll();
            //echo "<pre>";
            //print_r($photoReferencesTribute);
            //exit;
        }
    }
    // END FOR RELATED TRIBUTE CODE
    // START FOR BREADCRUMB
   // $breadcrumb = array();
   // $breadcrumb[] = l(t('Home'), '<front>', array('attributes' => array('title' => t('Home'))));
   // $breadcrumb[] = l(t('Gallery'), 'gallery', array('attributes' => array('title' => t('Gallery'))));
   // $breadcrumb[] = l(t('Photos'), 'photos', array('attributes' => array('title' => t('Photos'))));
  //  $breadcrumb[] = t('Photo Detail Page');
  //  drupal_set_breadcrumb($breadcrumb);

   // drupal_set_title(t('Photos'));
    // END FOR BREADCRUMB

    $output = theme('ghp_photosdetail', array(
        'photoReferencesVideo' => $photoReferencesVideo, 'videoCount' => $videoTotalCount,
        'photoReferencesCartoon' => $photoReferencesCartoon, 'cartoonCount' => $cartoonTotalCount,
        'photoReferencesAudio' => $photoReferencesAudio, 'audioCount' => $audioTotalCount,
        'photoReferencesPoster' => $photoReferencesPoster, 'posterCount' => $posterTotalCount,
        'photoReferencesStamp' => $photoReferencesStamp, 'stampCount' => $stampTotalCount,
        'photoReferencesCWMG' => $photoReferencesCWMG, 'cwmgCount' => $cwmgTotalCount,
        'photoReferencesTribute' => $photoReferencesTribute, 'tributeCount' => $tributeTotalCount, 'cwmgSections' => $cwmgSections));
    return $output;

    //$output = theme('ghp_photosdetail', array());
    // return $output;
}

function ghp_all_photos_list() {
    global $language, $base_url, $photo_query_condition;

    
    //Download Photo
    if (isset($_POST)) {
        if ((isset($_POST['submit']) == 'Download')):
            $image_name = base64_encode($_POST['image_name']);
            drupal_goto('photoDownload/"' . $image_name . '"');
        elseif (isset($_POST['cartoon_submit']) == 'cartoon_submit'):
            $cartoon_ID = base64_encode($_POST['cartoon_ID']);
            drupal_goto($base_url . "/cartoonDownload/" . $cartoon_ID);
        endif;
    }
if (isset($_POST['photoResetBtn']) == 'Reset') {
    $photoKeyword1 = $_POST['photoKeyword1'] = '';
    $photoKeyword = $_POST['photoKeyword'] = '';
    $selectedCatID = $_POST['photo_category'] = '';
    drupal_goto("photos-of-mahatma-gandhi"); 
	}

    // START FOR BREADCRUMB
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), '<front>', array('attributes' => array('title' => t('Home'))));
    $breadcrumb[] = l(t('Gallery'), 'gallery', array('attributes' => array('title' => t('Gallery'))));
    $breadcrumb[] = t('Photos');

    drupal_set_title(t('Photos'));
    drupal_set_breadcrumb($breadcrumb);
    // END FOR BREADCRUMB


    $cookieDuration = time() + 3600;

    // GET THE REFERENCE ARGUMENT
    $display_photos_of = arg(1);
    $display_photo_reference_id = arg(2);

    $FieldSelectedArray = unserialize(variable_get("ManageFieldSelectedArray"));
    bindQueryCondition($display_photos_of, $display_photo_reference_id);
    // END CODE
    // START FOR SEARCH TOPIC
    $searchQuery = "";
    $errorFlag = false;

    //SET PHOTO Query
    $mainSql = "SELECT pm.*,pd.*, YEAR(pm.photo_date) AS photoyear FROM photo_master pm 
        LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id 
        LEFT JOIN photo_category pc ON pm.category_id = pc.category_id 
        WHERE pd.language_code='" . $language->language . "' 
        AND pm.photo_publish_status = 'Y' 
        AND pm.photo_approve_status = 'Y' ";
if(isset($_POST['photoSearchBtn']))
{
	    if (!empty($_SESSION['photoModuleToken']) && $_SESSION['photoModuleToken'] == $_POST['photoModuleToken']) {
        if ((isset($_POST['photoKeyword1']) && (trim($_POST['photoKeyword1']) != '')) || (isset($_POST['photoKeyword']) && (trim($_POST['photoKeyword']) != ''))) {
		
		
            /* $photoKeyword = ((isset($_POST['photoKeyword1']) && (filtertext(strip_tags(trim($_POST['photoKeyword1'])))) != '')) ? trim($_POST['photoKeyword1']) : trim($_POST['photoKeyword']));*/
             
$photoKeyword = ((isset($_POST['photoKeyword1']) && (filtertext(strip_tags(trim($_POST['photoKeyword1'])))) != '')) ? filtertext(strip_tags(trim($_POST['photoKeyword1']))) : filtertext(strip_tags(trim($_POST['photoKeyword1'])));
            if (hasHtml($_POST['photoKeyword1'])) {
                form_set_error('photoKeyword1', t('Please enter valid search keyword'));
                $photoKeyword = $_POST['photoKeyword'] = $_POST['photoKeyword1'] = '';
                $errorFlag = true;
            } else if (hasHtml($_POST['photoKeyword'])) {
                form_set_error('photoKeyword', t('Please enter valid search keyword'));
                $photoKeyword = $_POST['photoKeyword'] = $_POST['photoKeyword1'] = '';
                $errorFlag = true;
            }

            if ($errorFlag == false) {

                $searchQuery = " AND (" . implode(" LIKE '%" . $photoKeyword . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $photoKeyword . "%' ) ";
            }
        }
    }
    else
    {
		form_set_error('photoKeyword', t('Invalid parameter passed'));
        $photoKeyword = $_POST['photoKeyword'] = $_POST['photoKeyword1'] = '';
	}
}	
    // END FOR SEARCH TOPIC    
	
    /*if (isset($_POST['photo_category'])) {
        $selected_category_id = filtertext(strip_tags($_POST['photo_category']));
        $_SESSION["selected_category_id"] = $selected_category_id;
        $selectedCatID = $_SESSION["selected_category_id"];
    } elseif (isset($_SESSION["selected_category_id"])) {
        $selectedCatID = $_SESSION["selected_category_id"];
    } else {
        unset($_SESSION["selected_category_id"]);
        $selectedCatID = '';
    }*/
	if (isset($_POST['photo_category'])) {
        $selected_category_id = (int)filtertext(strip_tags($_POST['photo_category']));
        $selectedCatID  = $selected_category_id;
        
    }

    if (isset($selectedCatID) && $selectedCatID != "") {
        $searchQuery .= " AND pm.category_id = '" . $selectedCatID . "' ";
    }
    /* js for Other Reference */
    drupal_add_js(drupal_get_path('module', 'cwmg_language') . '/js/jquery_172.js');
    /*
     * 
      if (arg(2) == 'stamp') {
      drupal_add_css(drupal_get_path('module', 'ghp_stamps') . '/css/thickbox.css');
      } else {
      drupal_add_css(drupal_get_path('module', 'cwmg_language') . '/css/thickbox.css');
      }
      drupal_add_js(drupal_get_path('module', 'cwmg_language') . '/js/thickbox.js');
      drupal_add_js(drupal_get_path('module', 'gandhi_heritage_site') . '/js/base_enc.js');
      drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/htmlentities.js');
      drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/jquery.mCustomScrollbar.css');

      drupal_add_css(drupal_get_path('module', 'ghp_gallery') . '/css/jshowcase.css');
 

      drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.tablesorter.min.js');
      drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.mCustomScrollbar.js');
      drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photo_detail.js');
     */


    // START FOR JPLAYER
    drupal_add_js(drupal_get_path('module', 'ghp_audios') . '/js/carousel.min.js');
    drupal_add_js(drupal_get_path('module', 'ghp_videos') . '/jplayer/js/jquery.jplayer.min.js');
    drupal_add_js(drupal_get_path('module', 'ghp_videos') . '/jplayer/js/jplayer.playlist.min.js');
    drupal_add_css(drupal_get_path('module', 'ghp_videos') . '/jplayer/skin/blue.monday/jplayer.blue.monday.css');
    // STOP FOR JPLAYER
	
	//new other ref poster css
	drupal_add_js(drupal_get_path('module', 'ghp_posters') . '/js/highslide-full.js' ); 
    drupal_add_css(drupal_get_path('module', 'ghp_chronology') . '/css/highslide.css');
	//end new poster css
	
    /* js for Standard View */
    drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/jquery.ad-gallery.css');
    drupal_add_css(drupal_get_path('module', 'ghp_stamps') . '/css/thickbox.css');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/thickbox.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.ad-gallery.js');
    // drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.scale.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.jcarousel.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.dependClass.js');
    // drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.lightbox-0.5.min.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photolist.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photo_reference_ajax.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/htmlentities.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.livequery.js');
    drupal_add_js(drupal_get_path('module', 'gandhi_heritage_site') . '/js/jquery.uniform.min.js');
    drupal_add_css(drupal_get_path('module', 'cwmg_language') . '/css/uniform.default.css');
    drupal_add_js(drupal_get_path('module', 'cwmg_language') . '/js/newprint.js');
 	drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photo_taging_front.js');
  	drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/tagging.css');
    drupal_add_css(drupal_get_path('module', 'ghp_references') . '/css/variorum_reference.css');
    drupal_add_js(drupal_get_path('module', 'ghp_references') . '/js/booksection_select.js');
    // SET VARIABLE FROM GLOBAL SEARCH KEYWORD


    if (isset($_REQUEST['header_search_keyword']) && $_REQUEST['header_search_keyword'] != '') {
        $global_search_keyword_value = $_REQUEST['header_search_keyword'];
        $global_search_condition = " AND ( " . implode(" LIKE '%" . $global_search_keyword_value . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $global_search_keyword_value . "%' ) ";
    } else {
        $global_search_keyword_value = '';
        $global_search_condition = '';
    }

    // GET PHOTO DATA FOR WCAG LEFT COLUMN
    if (isset($_POST['btnPhotoFilterYear']) && (isset($_POST['yearrangecheck']) && count($_POST['yearrangecheck']) > 0)) {

        $sql_limit = "";
        $total_selected_checkbox = count($_POST['yearrangecheck']);
        $selectedArray = array();
        for ($k = 0; $k < $total_selected_checkbox; $k++) {
            if ($_POST['yearrangecheck'][$k] != '') {
                list($from, $to) = explode("-", $_POST['yearrangecheck'][$k]);
                for ($x = $from; $x <= $to; $x++) {
                    $selectedArray[] = $x;
                }
            }
        }

        if (count($selectedArray) > 0) {
            $filterYear = implode(",", array_unique($selectedArray));

            $mainSql .= " AND YEAR(pm.photo_date) IN (" . $filterYear . ")  " . $photo_query_condition . " " . $global_search_condition;

            setcookie("photo_startyear", null, '', '/var/log/');
            setcookie("photo_endyear", null, '', '/var/log/');
            setcookie("photo_filteryear", base64_encode(serialize($filterYear)), $cookieDuration, '/var/log/');
            setcookie("photo_filteryearcheckbox", base64_encode(serialize($_POST['yearrangecheck'])), $cookieDuration, '/var/log/');
        }

        // GET PHOTO DATA FOR WCAG TOP SLIDER
    } else if (isset($_POST['searchphotos']) && (isset($_POST['startyear']) && $_POST['startyear'] != '') && (isset($_POST['endyear']) && $_POST['endyear'] != '')) {
        if ($_POST['startyear'] > $_POST['endyear']) {
            form_set_error('startyear', t('Start year should be less than End year.'));
        } else {
            $mainSql .= " AND YEAR(pm.photo_date) >=  '" . $_POST["startyear"] . "' AND YEAR(pm.photo_date) <=  '" . $_POST["endyear"] . "' ";

            setcookie("photo_startyear", base64_encode(serialize($_POST['startyear'])), $cookieDuration, '/var/log/');
            setcookie("photo_endyear", base64_encode(serialize($_POST['endyear'])), $cookieDuration, '/var/log/');
        }

        setcookie("photo_filteryear", null, '', '/var/log/');
        setcookie("photo_filteryearcheckbox", null, '', '/var/log/');

        $mainSql .= $photo_query_condition . " " . $global_search_condition;
        $sql_limit = "";
    } else if (isset($_POST['nextbutton']) || isset($_POST['prevbutton'])) {

        // CHECK 
        if ((isset($_COOKIE['photo_startyear']) && $_COOKIE['photo_startyear'] != null) && (isset($_COOKIE['photo_endyear']) && $_COOKIE['photo_endyear'] != null)) {

            $sql_limit = "";
            $mainSql .= " AND YEAR(pm.photo_date) >=  '" . unserialize(base64_decode($_COOKIE['photo_startyear'])) . "' AND YEAR(pm.photo_date) <=  '" . unserialize(base64_decode($_COOKIE['photo_endyear'])) . "'  " . $photo_query_condition . " " . $global_search_condition;
        } elseif ((isset($_COOKIE['photo_filteryear']) && $_COOKIE['photo_filteryear'] != null)) {

            $sql_limit = "";
            $mainSql .= " AND YEAR(pm.photo_date) IN (" . unserialize(base64_decode($_COOKIE['photo_filteryear'])) . ")  " . $photo_query_condition . " " . $global_search_condition;
        } else {

            $sql_limit = "";
            $mainSql .= $photo_query_condition . " " . $global_search_condition . " " . $searchQuery;
        }
        // GET PHOTO DATA
    } else {
        $sql_limit = " LIMIT 0,18";
        $mainSql .= $photo_query_condition . $searchQuery . " " . $global_search_condition;
    }
    $mainSql .= " ORDER BY pc.category_id ASC, pm.photo_order_number ASC " . $sql_limit;

    $photoRange = db_query($mainSql);

    $photoResultObject = $photoRange->fetchAll();
    $photoResultCount = $photoRange->rowCount();

    // fixed to set global search session condition only when some data found
    if( $photoResultCount == 0 )
    {
        $mainSql = str_replace($global_search_condition, " ", $mainSql);

        $photoRange = db_query($mainSql);

        $photoResultObject = $photoRange->fetchAll();
        $photoResultCount = $photoRange->rowCount();

        $global_search_keyword_value = '';
        $global_search_condition = '';
    }

    //echo "<pre>";	print_r($photoResultObject);		exit;
    $photoid = $photoResultObject[0]->photo_id;
    
    
    // GET TOTAL RECORDS
    if ((isset($_COOKIE['photo_startyear']) && $_COOKIE['photo_startyear'] != null) && (isset($_COOKIE['photo_endyear']) && $_COOKIE['photo_endyear'] != null)) {
    	
        $photoRangeCount = db_query("select pm.* from photo_master pm LEFT JOIN 
                                      photo_detail pd ON pm.photo_id = pd.photo_id 
                                      where pd.language_code='" . $language->language . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y'
                                      AND YEAR(pm.photo_date) >=  '" . unserialize(base64_decode($_COOKIE['photo_startyear'])) . "' 
                                      AND YEAR(pm.photo_date) <=  '" . unserialize(base64_decode($_COOKIE['photo_endyear'])) . "' " . $photo_query_condition . " " . $global_search_condition);
    } elseif ((isset($_COOKIE['photo_filteryear']) && $_COOKIE['photo_filteryear'] != null)) {
    	
        $photoRangeCount = db_query("select pm.* from photo_master pm LEFT JOIN 
                                    photo_detail pd ON pm.photo_id = pd.photo_id 
                                    where pd.language_code='" . $language->language . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y'
                                    AND YEAR(pm.photo_date) IN (" . unserialize(base64_decode($_COOKIE['photo_filteryear'])) . ")  " . $photo_query_condition . " " . $global_search_condition . " ORDER BY pm.photo_id ASC");
    } else {
    	
        $photoRangeCount = db_query("select pm.* from photo_master pm LEFT JOIN  photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $language->language . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' " . $photo_query_condition . $searchQuery . " " . $global_search_condition);
    }
    $totalPhotoCount = $photoRangeCount->rowCount();

    /* Active Tabs Array */
    $tabArray = array('video', 'cartoon', 'audio', 'poster', 'stamp', 'cwmg', 'tribute', 'variorum','book', 'journalbygandhi','journalbyother','othersection');
    $activeTab = arg(1);

    /* In Case Js is not active */

    
    
    /* In Case of reference header tabs clicked */
    if (isset($_POST['tab_header'])) {
        $photoid = key($_POST['tab_header']);
        $activeTabArr = $_POST['tab_header'][$photoid];
        //print_r($activeTabArr);
        $activeTab = key($activeTabArr);
    }

   if (isset($_POST['othersectioncat'])) {
        $photoid = key($_POST['othersectioncat']);
        $activeTab = 'othersection';
    }

    
    /* In Case of pagination of paricular tabs */
    if (isset($_GET['pager'])) {

        $activeTabArr = explode('_', $_GET['pager']);
        $activeTab = $activeTabArr[0];
        $photoid = $activeTabArr[1];
    }

    /* JS is not active closed */

    if (!in_array($activeTab, $tabArray)) {
        $activeTab = 'video';
    }

    if (isset($_POST['nextbutton'])) {
        $photoWCAGcounter = $_POST['photoWCAGCnt'] + 1;
        $_POST['CurrenIndex'] = $_POST['CurrenIndex'] + 1;  
        $photoid = $photoResultObject[$photoWCAGcounter]->photo_id;
        
    } elseif (isset($_POST['prevbutton'])) {
        $photoWCAGcounter = $_POST['photoWCAGCnt'] - 1;
        $_POST['CurrenIndex'] = $_POST['CurrenIndex'] - 1;
        $photoid = $photoResultObject[$photoWCAGcounter]->photo_id;
        
    } else {
        $photoWCAGcounter = 0;
    }

    $_SESSION['active_lang'] = $language->language;
    $_SESSION['active_module'] = "photo";
    /* fetch recored for other references */
    $ghp_otherreference_module_setting = explode(",",variable_get('ghp_otherreference_module_setting'));
  	$val_otherref = in_array('PH',$ghp_otherreference_module_setting);	
  	if(!empty($val_otherref)){
    $other_ref = ghp_photos_other_reference($photoid, $activeTab);
    }
    return theme('ghp_photoslist', array(
        'activeTab' => $activeTab,
        'photoid' => $photoid,
        'photoCount' => $photoResultCount,
        'photoListArray' => $photoResultObject,
        'totalPhotoCount' => $totalPhotoCount,
        'global_search_keyword' => $global_search_keyword_value,
        'selectedCatID' => $selectedCatID,
        'other_ref' => $other_ref,
            )
    );
}


function tag_list()
{
	/*
$sql = "SELECT * FROM photo_tag WHERE pic_id=" . $_POST[ 'pic_id' ] ;
$qry = db_query($sql)->fetchAll();
//print_r($qry);
$data['boxes'] = '';
$data['lists'] = '';

foreach ($qry as $qry) {
	
    $data['boxes'] .= '<div class="tagview" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y . 'px;" id="view_'.$qry->id.'">';
	$data['boxes'] .= '<div class="person" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y  . 'px;">' . $qry->name . '</div>';
	$data['boxes'] .= '<div class="square"></div>';
	$data['boxes'] .= '</div>';
	
	$data['lists'] .= '<li id="'.$qry->id.'"><a>' . $qry->name . '</a> (<a class="remove">Remove</a>)</li>';

    } 

echo json_encode( $data );*/



//print_r($qry);
$data['boxes'] = '';
$datalarge['boxeslarge'] = '';
$data['lists'] = '';
$datalarge['listslarge'] = '';

$data['copyright'] ='';


        $copyright_detail = "SELECT pm.photo_id, c.copyright_id, c.title, c.description FROM copyright c INNER JOIN photo_master pm ON c.copyright_id = pm.copyright_id where pm.photo_id = " . $_POST[ 'pic_id' ] . " group by pm.photo_id";
        $copyrights = db_query($copyright_detail)->fetchAll();

        if (!empty($copyrights)) {
            //$title = $copyrights[0]->title;
            $data['copyright'] .= $copyrights[0]->description;
            //echo json_encode ($data['copyright']);
            
        } else {
            
            $copyright_list = "SELECT cm.copyright_id, c.title, c.description FROM copyright c INNER JOIN copyright_module cm ON c.copyright_id=cm.copyright_id WHERE module_name = 'photos'";
            $copyrights = db_query($copyright_list)->fetchAll();

            if (!empty($copyrights)) {
                $data['copyright'] .= $copyrights[0]->description;
               // echo json_encode ($data['copyright']);
               
            } else {
                //Copyright from by default flag
                $copyright_default = "SELECT c.title, c.description FROM copyright c WHERE c.default_flag='1'";
                $copyrights = db_query($copyright_default)->fetchAll();
                $data['copyright'] .= $copyrights[0]->description;
              //  echo json_encode ($data['copyright']);
            }
        }



if($_POST['imgtype'] == 'normal')
{
//$sql = "SELECT * FROM photo_tag WHERE pic_id= '".$_POST[ 'pic_id' ]."' and imgtype = '".$_POST['imgtype']."' " ;
$sql = "SELECT * FROM photo_tag WHERE pic_id= '".$_POST[ 'pic_id' ]."' and imgtype = 'normal' " ;
$qry = db_query($sql)->fetchAll();
foreach ($qry as $qry) {
	
    $data['boxes'] .= '<div class="tagview" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y . 'px;" id="view_'.$qry->id.'">';
	$data['boxes'] .= '<div class="person" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y  . 'px;">' . $qry->name . '</div>';
	$data['boxes'] .= '<div class="square"></div>';
	$data['boxes'] .= '</div>';
	
	$data['lists'] .= '<li id="'.$qry->id.'" imgtype="'.$qry->imgtype.'"><a>' . $qry->name . '</a> (<a class="remove">Remove</a>)</li>';

    } 
echo json_encode( $data );    
}

else if($_POST['imgtype'] == 'large')
{
	$sql = "SELECT * FROM photo_tag WHERE pic_id= '".$_POST[ 'pic_id' ]."' and imgtype = 'large' " ;
$qrye = db_query($sql)->fetchAll();

foreach ($qrye as $qry) {
	
    $datalarge['boxeslarge'] .= '<div class="tagviewlarge" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y . 'px;" id="view_'.$qry->id.'">';
	$datalarge['boxeslarge'] .= '<div class="square"></div>';
	$datalarge['boxeslarge'] .= '<div class="person personBig" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y  . 'px;">' . $qry->name . '</div>';
	$datalarge['boxeslarge'] .= '</div>';
	
	$datalarge['listslarge'] .= '<li id="'.$qry->id.'" imgtype="'.$qry->imgtype.'"><a>' . $qry->name . '</a> (<a class="remove">Remove</a>)</li>';

    } 
	echo json_encode( $datalarge );    

}



       

	
}

/* Ajax call for reference tabs and load more reference tabs cases */

function ajax_pass_reference() {
    
    $photoid = (int)filtertext(strip_tags($_POST['currentImgIndex']));
    $activeTab = filtertext(strip_tags($_POST['activeTab']));
    $jsstatus = (int)filtertext(strip_tags($_POST['jsstatus']));
    
    if(isset($_SESSION['active_lang'])){
       $language->language = $_SESSION['active_lang'] ;
    }
    else{
        global $language;
    }
    
    /* check session for same photo id   */
    
    if ($_SESSION['photo_id'] == $photoid && $_SESSION['active_module']=='photo') {
        if ($_SESSION['main_result']) {
            $main_result = array();
            $main_result = $_SESSION['main_result'];

            $videoCount = (isset($main_result['videoTotalCount']) && $main_result['videoTotalCount'] != '') ? $main_result['videoTotalCount'] : '0';
            $cartoonCount = (isset($main_result['cartoonTotalCount']) && $main_result['cartoonTotalCount'] != '') ? $main_result['cartoonTotalCount'] : '0';
            $audioCount = (isset($main_result['audioTotalCount']) && $main_result['audioTotalCount'] != '') ? $main_result['audioTotalCount'] : '0';
            $posterCount = (isset($main_result['posterTotalCount']) && $main_result['posterTotalCount'] != '') ? $main_result['posterTotalCount'] : '0';
            $stampCount = (isset($main_result['stampTotalCount']) && $main_result['stampTotalCount'] != '') ? $main_result['stampTotalCount'] : '0';
            $cwmgCount = (isset($main_result['cwmgTotalCount']) && $main_result['cwmgTotalCount'] != '') ? $main_result['cwmgTotalCount'] : '0';
            $tributeCount = (isset($main_result['tributeTotalCount']) && $main_result['tributeTotalCount'] != '') ? $main_result['tributeTotalCount'] : '0';
            $variorumCount = (isset($main_result['variorumTotalCount']) && $main_result['variorumTotalCount'] != '') ? $main_result['variorumTotalCount'] : '0';
            $bookCount = (isset($main_result['bookTotalCount']) && $main_result['bookTotalCount'] != '') ? $main_result['bookTotalCount'] : '0';
            $journalgandhiCount = (isset($main_result['journalgandhiTotalCount']) && $main_result['journalgandhiTotalCount'] != '') ? $main_result['journalgandhiTotalCount'] : '0';
            $journalotherCount = (isset($main_result['journalotherTotalCount']) && $main_result['journalotherTotalCount'] != '') ? $main_result['journalotherTotalCount'] : '0';
            $othersectionCount = (isset($main_result['booksectionTotalCount']) && $main_result['booksectionTotalCount'] != '') ? $main_result['booksectionTotalCount'] : '0';
            
            $output = '';
            $output .= theme('ghp_tab_header_reference', array(
                'videoCount' => $videoCount,
                'cartoonCount' => $cartoonCount,
                'audioCount' => $audioCount,
                'posterCount' => $posterCount,
                'stampCount' => $stampCount,
                'cwmgCount' => $cwmgCount,
                'tributeCount' => $tributeCount,
                'variorumCount' => $variorumCount,
                'bookCount' => $bookCount,
                'journalbyotherCount' => $journalotherCount,
                'journalbygandhiCount' => $journalgandhiCount,
                'othersectionCount' => $othersectionCount,
                'jsstatus' => $jsstatus,
                'TabDisplayOrder' => $main_result['TabDisplayOrder']
                    )
            );

            $output .= "<div id='refrence_details'><div class='refrence_details'>";
            if ($activeTab == 'video'):
                $output .= theme('ghp_video_reference', array(
                    'ReferencesVideo' => $main_result['photoReferencesVideo'],
                    'videoCount' => $main_result['videoTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );

            elseif ($activeTab == 'cartoon'):
                $output .= theme('ghp_cartoon_reference', array(
                    'ReferencesCartoon' => $main_result['photoReferencesCartoon'],
                    'cartoonCount' => $main_result['cartoonTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );

            elseif ($activeTab == 'audio'):
                $output .= theme('ghp_audio_reference', array(
                    'ReferencesAudio' => $main_result['photoReferencesAudio'],
                    'audioCount' => $main_result['audioTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );
            elseif ($activeTab == 'poster'):
                $output .= theme('ghp_poster_reference', array(
                    'ReferencesPoster' => $main_result['photoReferencesPoster'],
                    'posterCount' => $main_result['posterTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );
            elseif ($activeTab == 'stamp'):
                $output .= theme('ghp_stamp_reference', array(
                    'ReferencesStamp' => $main_result['photoReferencesStamp'],
                    'stampCount' => $main_result['stampTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );
            elseif ($activeTab == 'cwmg'):
                $output .= theme('ghp_cwmg_reference', array(
                    'ReferencesCWMG' => $main_result['photoReferencesCWMG'],
                    'cwmgCount' => $main_result['cwmgTotalCount'],
                    'cwmgSections' => $main_result['cwmgSections'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );
            elseif ($activeTab == 'tribute'):
                $output .= theme('ghp_tribute_reference', array(
                    'ReferencesTribute' => $main_result['photoReferencesTribute'],
                    'tributeCount' => $main_result['tributeTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );
            elseif ($activeTab == 'variorum'):

                $output .= theme('ghp_variorum_reference', array(
                    'ReferencesVariorum' => $main_result['photoReferencesVariorum'],
                    'variorumCount' => $main_result['variorumTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus,
                    'CurrentItemId' => $photoid
                        )
                );
              elseif ($activeTab == 'book'):
				
                $output .= theme('ghp_book_reference', array(
                    'ReferencesBook' => $main_result['photoReferencesBook'],
                    'bookCount' => $main_result['bookTotalCount'],
					'booksSections' => $main_result['booksSections'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus,
                    'CurrentItemId' => $photoid
                        )
                );
                
                 elseif ($activeTab == 'journalbygandhi'):
				
                $output .= theme('ghp_journal_by_gandhi_reference', array(
                    'ReferencesJournalgandhi' => $main_result['photoReferencesJournalgandhi'],
                    'journalbygandhiCount' => $main_result['journalgandhiTotalCount'],
                    'journalgandhi_section_format' => $main_result['journalgandhi_section_format'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus,
                    'CurrentItemId' => $photoid
                        )
                );
                
                 elseif ($activeTab == 'journalbyother'):
                    $output .= theme('ghp_journal_by_other_reference', array(
                        'ReferencesJournalother' => $main_result['photoReferencesJournalother'],
                        'journalbyotherCount' => $main_result['journalotherTotalCount'],
                        'journalother_section_format' => $main_result['journalother_section_format'],
                        'activeTab' => $activeTab,
                        'jsstatus' => $jsstatus,
                        'CurrentItemId' => $photoid
                        )
                );   
                 elseif ($activeTab == 'othersection'):
                    $output .= theme('ghp_othersection_reference', array(
                        'Referencesothersection' => $main_result['photoReferencesBooksection'],
                        'othersectionCount' => $main_result['booksectionTotalCount'],
                        'booksection_section_format' => $main_result['booksection_section_format'],
                        'activeTab' => $activeTab,
                        'jsstatus' => $jsstatus,
                        'CurrentItemId' => $photoid,
                            )
                    );
                 
            else :
                $output .= theme('ghp_video_reference', array(
                    'ReferencesVideo' => $main_result['photoReferencesVideo'],
                    'videoCount' => $main_result['videoTotalCount'],
                    'activeTab' => $activeTab,
                    'jsstatus' => $jsstatus
                        )
                );

            endif;
            $output .= "</div></div>";
            print_r($output);
            exit;
        }
    }
    $_SESSION['photo_id'] = $photoid;
    $_SESSION['active_module'] = "photo";
    $_SESSION['main_result'] = '';

    ghp_photos_other_reference($photoid, $activeTab);
}

function ghp_photos_other_reference($photoid, $activeTab) {

    global $base_path, $language;
    global $base_url;
    $main_result = array();
    
    if(isset($_SESSION['active_lang'])){
       $language->language = $_SESSION['active_lang'] ;
    }
    else{
        global $language;
    }
    
    //SET PHOTO Query
    $mainSql = "SELECT pm.*,pd.*, YEAR(pm.photo_date) AS photoyear FROM photo_master pm 
        LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id 
        LEFT JOIN photo_category pc ON pm.category_id = pc.category_id 
        WHERE pd.language_code='" . $language->language . "' 
        AND pm.photo_publish_status = 'Y' 
        AND pm.photo_id ='" . $photoid . "' 
        AND pm.photo_approve_status = 'Y' ";

    $photoRange = db_query($mainSql);
    $photoResultObject = $photoRange->fetchAll();
    $photoResultCount = $photoRange->rowCount();
    $photoid = $photoResultObject[0]->photo_id;

    // Get Reference Video Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Video');
    $select->orderBy('id', 'ASC');
    $photoReferencesVideo = $select->execute()->fetchAll();
    $main_result['photoReferencesVideo'] = $photoReferencesVideo;

    $videoTotalCount = '';
    if (count($photoReferencesVideo) > 0) {
        $videoWithItems = explode(';', $photoReferencesVideo[0]->section_format);

        // seperate items
        for ($i = 0; $i < count($videoWithItems); $i++) {
            if ($videoWithItems[$i] != '')
                $videoWithIdArray[] = explode(':', $videoWithItems[$i]);
        }
        // seperate video ids
        /* for ($i = 0; $i < count($videoWithCategory); $i++) {
            $videoWithIdArray[] = explode(':', $videoWithCategory[$i][1]);

        } */
        // Murger Video ids
        $videoWithId = '';
        if (count($videoWithIdArray) > 1) {
            $count = count($videoWithIdArray) - 1;
            for ($i = 0; $i < count($videoWithIdArray); $i++) {
                if ($count == $i) {
                    $videoWithId .= $videoWithIdArray[$i][1];
                } else {
                    $videoWithId .= $videoWithIdArray[$i][1] . ',';
                }
            }
        } else {
            $videoWithId = $videoWithIdArray[0][1];
        }
		
        // Sort Video ids
        if ($videoWithId != '') {
            $videoId = explode(',', $videoWithId); // asort(explode(',', $photoWithId));            
            sort($videoId);
            /* $videoWithId = implode(',', array_unique($videoId)); */
 			$videoWithId = trim(implode(',', array_unique($videoId)),",");
          
            // Get reference video from database
            $select = db_select('video_master', 'vm');
            $select->join('video_detail', 'vd', 'vd.video_id = vm.video_id');
            $select->fields('vm', array('video_id', 'category_id', 'video_filename', 'video_image', 'video_publish_status', 'video_approve_status'));
            $select->fields('vd', array('video_detail_id', 'language_code', 'videographer_name', 'video_title', 'video_description', 'video_duration'));
            $select->condition('video_publish_status', 'Y');
            $select->condition('video_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' vm.video_id IN (' . $videoWithId . ')');
            $select->orderBy('video_id', 'ASC');
            $selectTotal = $select;
            
            $videoTotalCount = $selectTotal->execute()->rowCount();
            $main_result['videoTotalCount'] = $videoTotalCount;
            $select = $select->extend('PagerDefault')->element(3)->limit(3);
            $photoReferencesVideo = $select->execute()->fetchAll();
            $main_result['photoReferencesVideo'] = $photoReferencesVideo;
           }
    }
    // End of related Video item code
    // 
    // Get Related Cartoons from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Cartoon');
    $select->orderBy('id', 'ASC');
    $photoReferencesCartoon = $select->execute()->fetchAll();

    $cartoonTotalCount = '';
    if (count($photoReferencesCartoon) > 0) {
        $cartoonWithItems = $photoReferencesCartoon[0]->section_format;

        // Sort cartoon ids
        if ($cartoonWithItems != '') {
            $cartoonId = explode(',', $cartoonWithItems); // asort(explode(',', $photoWithId));            
            sort($cartoonId);
            /* $cartoonWithId = implode(',', array_unique($cartoonId)); */
			$cartoonWithId = trim(implode(',', array_unique($cartoonId)),",");

            // Get reference cartoon from database
            $select = db_select('cartoon_master', 'cm');
            $select->join('cartoon_detail', 'cd', 'cd.cartoon_id = cm.cartoon_id');
            $select->fields('cm', array('cartoon_id', 'category_id', 'cartoon_filename', 'cartoon_flipimage', 'cartoon_publish_status', 'cartoon_approve_status'));
            $select->fields('cd', array('cartoon_detail_id', 'language_code', 'cartoon_title', 'cartoon_description'));
            $select->condition('cartoon_publish_status', 'Y');
            $select->condition('cartoon_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' cm.cartoon_id IN (' . $cartoonWithId . ')');
            $select->orderBy('cartoon_id', 'ASC');
            $selectTotal = $select;
            $cartoonTotalCount = $selectTotal->execute()->rowCount();
            $main_result['cartoonTotalCount'] = $cartoonTotalCount;
            $select = $select->extend('PagerDefault')->element(4)->limit(3);
            $photoReferencesCartoon = $select->execute()->fetchAll();
            $main_result['photoReferencesCartoon'] = $photoReferencesCartoon;
        }
    }
    // End of related Cartoon code !!
    // 
    // Get Reference Audio Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Audio');
    $select->orderBy('id', 'ASC');
    $photoReferencesAudio = $select->execute()->fetchAll();

    $audioTotalCount = '';
    if (count($photoReferencesAudio) > 0) {
        $audioWithItems = explode(';', $photoReferencesAudio[0]->section_format);
        // seperate items
        for ($i = 0; $i < count($audioWithItems); $i++) {
            if ($audioWithItems[$i] != '')
                $audioWithIdArray[] = explode(':', $audioWithItems[$i]);
        }
        // seperate audio ids
        /* for ($i = 0; $i < count($audioWithCategory); $i++) {
            $audioWithIdArray[] = explode(':', $audioWithCategory[$i][1]);
        } */
		// Murger audio ids
        $audioWithId = '';
        if (count($audioWithIdArray) > 1) {
            $count = count($audioWithIdArray) - 1;
            for ($i = 0; $i < count($audioWithIdArray); $i++) {
                if ($count == $i) {
                    $audioWithId .= $audioWithIdArray[$i][1];
                } else {
                    $audioWithId .= $audioWithIdArray[$i][1] . ',';
                }
            }
        } else {
            $audioWithId = $audioWithIdArray[0][1];
        }
        // Sort Audio ids
        if ($audioWithId != '') {
            $audioId = explode(',', $audioWithId); // asort(explode(',', $photoWithId));            
            sort($audioId);
            /* $audioWithId = implode(',', array_unique($audioId)); */
			$audioWithId = trim(implode(',', array_unique($audioId)),",");
			
            // Get reference audio from database
            $select = db_select('audio_master', 'am');
            $select->join('audio_detail', 'ad', 'ad.audio_id = am.audio_id');
            $select->fields('am', array('audio_id', 'category_id', 'audio_filename', 'audio_date', 'audio_publish_status', 'audio_approve_status'));
            $select->fields('ad', array('audio_detail_id', 'language_code', 'audio_title', 'audio_description', 'audio_duration', 'audio_place', 'ActualVolID', 'VolumePageRange'));
            $select->addExpression("DATE_FORMAT(am.audio_date,'%d/%m/%Y')", 'audio_date');
            $select->condition('audio_publish_status', 'Y');
            $select->condition('audio_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' am.audio_id IN (' . $audioWithId . ')');
            $select->orderBy('audio_id', 'ASC');
            $selectTotal = $select;
            $audioTotalCount = $selectTotal->execute()->rowCount();
            $main_result['audioTotalCount'] = $audioTotalCount;
            $select = $select->extend('PagerDefault')->element(5)->limit(3);
            $photoReferencesAudio = $select->execute()->fetchAll();
            $main_result['photoReferencesAudio'] = $photoReferencesAudio;
        }
    }
    // End of related Audio item code    
    // 
    // Get Related Poster from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Poster');
    $select->orderBy('id', 'ASC');
    $photoReferencesPoster = $select->execute()->fetchAll();

    $posterTotalCount = '';
    if (count($photoReferencesPoster) > 0) {
        $posterWithItems = $photoReferencesPoster[0]->section_format;

        // Sort poster ids
        if ($posterWithItems != '') {
            $posterId = explode(',', $posterWithItems); // asort(explode(',', $photoWithId));            
            sort($posterId);
            /* $posterWithId = implode(',', array_unique($posterId)); */
			$posterWithId = trim(implode(',', array_unique($posterId)),",");

            // Get reference poster from database
            $select = db_select('poster_master', 'pm');
            $select->join('poster_detail', 'pd', 'pd.poster_id = pm.poster_id');
            $select->fields('pm', array('poster_id', 'category_id', 'poster_filename', 'poster_publish_status', 'poster_approve_status'));
            $select->fields('pd', array('poster_detail_id', 'language_code', 'poster_title', 'poster_description'));
            $select->condition('poster_publish_status', 'Y');
            $select->condition('poster_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' pm.poster_id IN (' . $posterWithId . ')');
            $select->orderBy('poster_id', 'ASC');
            $selectTotal = $select;
            $posterTotalCount = $selectTotal->execute()->rowCount();
            $main_result['posterTotalCount'] = $posterTotalCount;
            $select = $select->extend('PagerDefault')->element(6)->limit(3);
            $photoReferencesPoster = $select->execute()->fetchAll();
            $main_result['photoReferencesPoster'] = $photoReferencesPoster;
        }
    }
    // End of related Poster code !!
    //
    // Get Related Stamp from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Stamp');
    $select->orderBy('id', 'ASC');
    $photoReferencesStamp = $select->execute()->fetchAll();

    $stampTotalCount = '';
    if (count($photoReferencesStamp) > 0) {
        $stampWithItems = $photoReferencesStamp[0]->section_format;

        // Sort poster ids
        if ($stampWithItems != '') {
            $stampId = explode(',', $stampWithItems); // asort(explode(',', $photoWithId));            
            sort($stampId);
            /* $stampWithId = implode(',', array_unique($stampId)); */
			$stampWithId = trim(implode(',', array_unique($stampId)),",");

            // Get reference poster from database
            $select = db_select('stamp_master', 'sm');
            $select->join('stamp_detail', 'sd', 'sd.stamp_id = sm.stamp_id');
            $select->join('category', 'c', 'sm.category_id  = c.category_id');
            $select->fields('sm', array('stamp_id', 'category_id', 'stamp_filename', 'stamp_publish_status', 'stamp_approve_status'));
            $select->fields('sd', array('stamp_detail_id', 'language_code', 'stamp_title', 'stamp_description'));
            $select->fields('c', array('slug'));
            $select->condition('stamp_publish_status', 'Y');
            $select->condition('stamp_approve_status', 'Y');
            $select->condition('language_code', $language->language);
            $select->where(' sm.stamp_id IN (' . $stampWithId . ')');
            $select->orderBy('stamp_id', 'ASC');
            $selectTotal = $select;
            $stampTotalCount = $selectTotal->execute()->rowCount();
            $main_result['stampTotalCount'] = $stampTotalCount;
            $select = $select->extend('PagerDefault')->element(7)->limit(4);
            $photoReferencesStamp = $select->execute()->fetchAll();
            $main_result['photoReferencesStamp'] = $photoReferencesStamp;
        }
    }
    // End of related Stamp code !!
    //
    // Get Reference CWMG Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Volume');
    $select->orderBy('id', 'ASC');
    $photoReferencesCWMG = $select->execute()->fetchAll();

    $cwmgTotalCount = $cwmgSections = '';
    if (count($photoReferencesCWMG) > 0) {
        $cwmgSections = $photoReferencesCWMG[0]->section_format;
        $main_result['cwmgSections'] = $cwmgSections;
        $cwmgWithItems = explode(';', $photoReferencesCWMG[0]->section_format);
        // seperate items
        for ($i = 0; $i < count($cwmgWithItems); $i++) {
            if ($cwmgWithItems[$i] != '')
                $cwmgWithIdArray[] = explode(':', $cwmgWithItems[$i]);
			
			
        }
		
        // seperate audio ids
        /* for ($i = 0; $i < count($cwmgWithCategory); $i++) {
            $cwmgWithIdArray[] = explode(':', $cwmgWithCategory[$i][0]);
            //$cwmgWithIdArray[] = $cwmgWithCategory[$i][0];
			 
			 //print_r($cwmgWithCategory[$i][0]);
        } */
		
        // Murger audio ids
        $cwmgWithId = '';
        if (count($cwmgWithIdArray) > 1) {
            $count = count($cwmgWithIdArray) - 1;
            for ($i = 0; $i < count($cwmgWithIdArray); $i++) {
                if ($count == $i) {
                    $cwmgWithId .= $cwmgWithIdArray[$i][0];
                } else {
                    $cwmgWithId .= $cwmgWithIdArray[$i][0] . ',';
                }
            }
        } else {
            $cwmgWithId = $cwmgWithIdArray[0][0];
        }

        // Sort Audio ids
        if ($cwmgWithId != '') {
            $cwmgId = explode(',', $cwmgWithId); // asort(explode(',', $photoWithId));            
            sort($cwmgId);
            /* $cwmgWithId = implode(',', array_unique($cwmgId)); */
			$cwmgWithId = trim(implode(',', array_unique($cwmgId)),",");

            $select_cwmg_language = db_select('cwmg_language', 'cwmgl');
            $select_cwmg_language->fields('cwmgl', array('language_id'));
            $select_cwmg_language->condition('language_code', $language->language);
            $cwmg_language_data_obj = $select_cwmg_language->execute();
            $cwmg_language_count = $cwmg_language_data_obj->rowCount();
            $cwmg_language_data = $cwmg_language_data_obj->fetchAssoc();

            if( $cwmg_language_count>0 && !empty($cwmg_language_data['language_id']) )
            {
                // Get reference audio from database
                $select = db_select('volume_master', 'vm');
                $select->fields('vm', array('ActualVol', 'LanguageID', 'VolumeName', 'ActualDisplayPages', 'NumberOfPages'));
                $select->condition('volume_publish_status', 'Y');
				$select->condition('volume_approve_status', 'Y');
                $select->condition('LanguageID', $cwmg_language_data['language_id']);
                $select->where(' vm.ActualVol IN (' . $cwmgWithId . ')');
                $select->orderBy('ActualVol', 'ASC');
                $selectTotal = $select;
                $cwmgTotalCount = $selectTotal->execute()->rowCount();
                $main_result['cwmgTotalCount'] = $cwmgTotalCount;
                $select = $select->extend('PagerDefault')->element(8)->limit(3);
                $photoReferencesCWMG = $select->execute()->fetchAll();
                $main_result['photoReferencesCWMG'] = $photoReferencesCWMG;
            }
        }
    }
    // End of related CWMG code
    //Get Reference Tribute Items
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Tribute');
    $select->orderBy('id', 'ASC');
    $photoReferencesTribute = $select->execute()->fetchAll();

    $tributeTotalCount = '';
    if (count($photoReferencesTribute) > 0) {
        $tributeWithItems = $photoReferencesTribute[0]->section_format;

        // Sort tributes ids
        if ($tributeWithItems != '') {
            $tributeId = explode(',', $tributeWithItems);
            sort($tributeId);
            /* $tributeWithId = implode(',', array_unique($tributeId)); */
			$tributeWithId = trim(implode(',', array_unique($tributeId)),",");

            // Get reference tributes from database
            $select = db_select('tribute_detail', 'td');
            $select->join('tribute_master', 'tm', 'tm.tributeid  = td.tributeid');
            $select->join('tributeperson_detail', 'tpd', 'tpd. personid  = tm.tributeby');
            $select->join('tributeperson_master', 'tpm', 'tpm.personid  = tpd.personid');
            $select->fields('tpm', array('personphoto'));
            $select->fields('tpd', array('personid', 'firstname', 'lastname', 'personprofile'));
            $select->fields('td', array('id', 'tributeid', 'title', 'tributetext', 'tributeplace'));
            $select->where('tpm.personid IN (' . $tributeWithId . ')');
            $select->condition('td.languagecode', $language->language);
            $select->condition('tpd.languagecode', $language->language);
            $select->condition('td.tributepublishstatus', "Y");
            $select->condition('td.tributeapprovedstatus', "Y");
            $select->orderBy('personid', 'ASC');
            $selectTotal = $select;
            $tributeTotalCount = $selectTotal->execute()->rowCount();
            $main_result['tributeTotalCount'] = $tributeTotalCount;
            $select = $select->extend('PagerDefault')->element(10)->limit(3);
            $photoReferencesTribute = $select->execute()->fetchAll();
            $main_result['photoReferencesTribute'] = $photoReferencesTribute;
        }
    }
    // End For Related Tribute Code
    // 
    //
    // Get Related variorum from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Variorum');
    $select->orderBy('id', 'ASC');
    $photoReferencesVariorum = $select->execute()->fetchAll();

    $variorumTotalCount = '';
    if (count($photoReferencesVariorum) > 0) {
        $variorumWithItems = $photoReferencesVariorum[0]->section_format;

        // Sort poster ids
        if ($variorumWithItems != '') {
            $variorumId = explode(',', $variorumWithItems); // asort(explode(',', $photoWithId));            
            sort($variorumId);

            /* $variorumWithId = implode(',', array_unique($variorumId)); */
			$variorumWithId = trim(implode(',', array_unique($variorumId)),",");
			
            // Get reference of veroriam table
            $select = db_select('veroriam_master', 'vm');
            $select->join('veroriam_document_type', 'vdt', 'vm.document_type = vdt.id');
            $select->join('veroriam_language', 'vl', 'vm.language = vl.id');
            $select->fields('vm', array('id', 'serial_number', 'serial_number_int', 'treatment', 'vdate', 'document_type', 'document_type_text', 'document_copy', 'no_enclosures', 'from_name', 'to_name', 'from_place', 'to_place', 'missing_letters', 'language', 'no_page', 'classification', 'subject', 'status', 'created'));
            $select->fields('vdt', array('title'));
            $select->fields('vl', array('title'));
            $select->where(' vm.id IN (' . $variorumWithId . ')');
            $select->condition('vl.tag', $language->language);
            $select->orderBy('id', 'ASC');
            $selectTotal = $select;

            $variorumTotalCount = $selectTotal->execute()->rowCount();
            $main_result['variorumTotalCount'] = $variorumTotalCount;
            $select = $select->extend('PagerDefault')->element(9)->limit(3);
            $photoReferencesVariorum = $select->execute()->fetchAll();
            $main_result['photoReferencesVariorum'] = $photoReferencesVariorum;
        }
    }
    // End of related variorum code !!
    
    
      // Get Related keytext(book) from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'Book');
    $select->orderBy('id', 'ASC');
    $photoReferencesBook = $select->execute()->fetchAll();
	
    $bookTotalCount = '';
    if (count($photoReferencesBook) > 0) {
        $booksSections = $photoReferencesBook[0]->section_format;
		//added actualbook  seprate code
		$main_result['booksSections'] = $booksSections;
        $bookWithItems = explode(';', $photoReferencesBook[0]->section_format);
        // seperate items
        for ($i = 0; $i < count($bookWithItems); $i++) {
            if ($bookWithItems[$i] != '')
                $bookWithIdArray[] = explode(':', $bookWithItems[$i]);
			
			
        }
		
		 // Murger books ids
        $bookWithId = '';
        if (count($bookWithIdArray) > 1) {
             $count = count($bookWithIdArray) - 1;
            for ($i = 0; $i < count($bookWithIdArray); $i++) {
                if ($count == $i) {
                    $bookWithId .= $bookWithIdArray[$i][0];
                } else {
                    $bookWithId .= $bookWithIdArray[$i][0] . ',';
                }
            }
        } else {
            $bookWithId = $bookWithIdArray[0][0];
        }
		//end of actualbook part
		
        // Sort poster ids
        if ($bookWithId != '') {
            $bookId = explode(',', $bookWithId); // asort(explode(',', $photoWithId));            
            sort($bookId);

            /* $bookWithId = implode(',', array_unique($bookId)); */
			$bookWithId = trim(implode(',', array_unique($bookId)),",");

            // Get reference of books table
            $select = db_select('books', 'b');
            $select->join('books_language', 'bl', 'b.LanguageID = bl.language_id');
            $select->fields('bl', array('language_id','language_code'));
            $select->fields('b', array('BookId', 'ActualBook', 'Title', 'Author', 'Latitude', 'Longitude', 'PlaceName', 'QuotedIn', 'PublishDate', 'StartDate', 'EndDate','BookImg','book_order_number','front_cover','copyright','table_content','first_page','index_number','back_cover'));
			$select->condition('book_publish_status', 'Y');
            $select->condition('book_approve_status', 'Y');
            $select->where(' b.BookId IN (' . $bookWithId . ')');
            $select->condition('bl.language_code', $language->language);
            $select->orderBy('BookId', 'ASC');
            $selectTotal = $select;

            $bookTotalCount = $selectTotal->execute()->rowCount();
            $main_result['bookTotalCount'] = $bookTotalCount;
            $select = $select->extend('PagerDefault')->element(11)->limit(3);
            $photoReferencesBook = $select->execute()->fetchAll();
            $main_result['photoReferencesBook'] = $photoReferencesBook;
        }
    }
    // End of related keytext(Book) code !!
   
    // Get Related journal by gandhi book from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'JournalByGandhiji');
    $select->orderBy('id', 'ASC');
    $photoReferencesJournalgandhi = $select->execute()->fetchAll();
	
	//print_r($photoReferencesJournalgandhi);
	

    $journalgandhiTotalCount = '';
    if (count($photoReferencesJournalgandhi) > 0) {
        $journalgandhiWithItems = $photoReferencesJournalgandhi[0]->section_format;

        // Sort poster ids
        if ($journalgandhiWithItems != '') {
            // $journalgandhiId = explode(',', $journalgandhiWithItems); // asort(explode(',', $photoWithId));            
            // sort($journalgandhiId);

            // $journalgandhiWithId = implode(',', array_unique($journalgandhiId));

            $journalgandhiArray = explode(';', $journalgandhiWithItems);

            $journalIDsArray = array();
            for ($i = 0; $i < count($journalgandhiArray); $i++) {
                $journalVolumeIssuePagesData = explode(':', $journalgandhiArray[$i]);
                $journalVolumeIssueIDsArray = explode('-', $journalVolumeIssuePagesData[0]);
                $journalIDsArray[$i] = $journalVolumeIssueIDsArray[0];
            }
            sort($journalIDsArray);

            $journalgandhiWithId = trim(implode(',', array_unique($journalIDsArray)),",");

            // Get reference of journal by gandhi table
            $select = db_select('journal_master', 'jm');
			$select->fields( 'jm', array('journal_id', 'journal_cover_image') );
			$select->fields( 'jd', array('journal_title') );
			$select->leftJoin('journal_detail', 'jd', 'jm.journal_id = jd.journal_id');
			// $select->condition('language_code', $language->language);
			$select->condition('journal_publish_status', 'Y');
			$select->condition('journal_approve_status', 'Y');
			$select->condition('journal_parent_id', '7');
			$select->where(' jm.journal_id IN (' . $journalgandhiWithId . ')');
            $select->orderBy('journal_id', 'ASC');
            $selectTotal = $select;
            $journalgandhiTotalCount = $selectTotal->execute()->rowCount();
            $main_result['journalgandhiTotalCount'] = $journalgandhiTotalCount;
            $select = $select->extend('PagerDefault')->element(12)->limit(4);
            $photoReferencesJournalgandhi = $select->execute()->fetchAll();
            $main_result['photoReferencesJournalgandhi'] = $photoReferencesJournalgandhi;

            $main_result['journalgandhi_section_format'] = $journalgandhiWithItems;

        }
    }
    // end of journal by gandhi
    
    // Get Related journal by other book from database
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'JournalByOthers');
    $select->orderBy('id', 'ASC');
    $photoReferencesJournalother = $select->execute()->fetchAll();

    $journalotherTotalCount = '';
    if (count($photoReferencesJournalother) > 0) {
        $journalotherWithItems = $photoReferencesJournalother[0]->section_format;

        // Sort poster ids
        if ($journalotherWithItems != '') {
            // $journalotherId = explode(',', $journalotherWithItems); // asort(explode(',', $photoWithId));            
            // sort($journalotherId);

            // $journalotherWithId = implode(',', array_unique($journalotherId));

            $journalotherArray = explode(';', $journalotherWithItems);

            $journalIDsArray = array();
            for ($i = 0; $i < count($journalotherArray); $i++) {
                $journalVolumeIssuePagesData = explode(':', $journalotherArray[$i]);
                $journalVolumeIssueIDsArray = explode('-', $journalVolumeIssuePagesData[0]);
                $journalIDsArray[$i] = $journalVolumeIssueIDsArray[0];
            }
            sort($journalIDsArray);

            $journalotherWithId = trim(implode(',', array_unique($journalIDsArray)),",");

            // Get reference of journal by gandhi table
            $select = db_select('journal_master', 'jm');
			$select->fields( 'jm', array('journal_id', 'journal_cover_image') );
			$select->fields( 'jd', array('journal_title') );
			$select->leftJoin('journal_detail', 'jd', 'jm.journal_id = jd.journal_id');
			// $select->condition('language_code', $language->language);
			$select->condition('journal_publish_status', 'Y');
			$select->condition('journal_approve_status', 'Y');
			$select->condition('journal_parent_id', '8');
			$select->where(' jm.journal_id IN (' . $journalotherWithId . ')');
            $select->orderBy('journal_id', 'ASC');
            $selectTotal = $select;
            $journalotherTotalCount = $selectTotal->execute()->rowCount();
            $main_result['journalotherTotalCount'] = $journalotherTotalCount;
            $select = $select->extend('PagerDefault')->element(13)->limit(4);
            $photoReferencesJournalother = $select->execute()->fetchAll();
            $main_result['photoReferencesJournalother'] = $photoReferencesJournalother;

            $main_result['journalother_section_format'] = $journalotherWithItems;
            
        }
    }
    
    // Get Related other booksections  list
    
    $select = db_select('photo_master', 'pm');
    $select->join('photo_references', 'pr', 'pr.ref_id = pm.photo_id');
    $select->fields('pm', array('photo_date', 'created_by'));
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $photoid);
    $select->condition('module_name', 'Photo');
    $select->condition('section_name', 'FundamentalWork');
    $select->orderBy('id', 'ASC');
    $photoReferencesbooksections = $select->execute()->fetchAll();

    
    $booksectionsTotalCount = '';
    if (count($photoReferencesbooksections) > 0) {
        $bookWithItems = explode(';', $photoReferencesbooksections[0]->section_format);
        
        
        // seperate items
        for ($i = 0; $i < count($bookWithItems); $i++) {
            if ($bookWithItems[$i] != '')
                $bookWithCategory[] = explode(':', $bookWithItems[$i]);
        }
		
        $listbooksresult = array();
        $booksectionTotalCount = 0;
        // seperate audio ids
        for ($i = 0; $i < count($bookWithCategory); $i++) {
            
            $term = taxonomy_term_load($bookWithCategory[$i][0]);
            $Cat_name = $term->name;
            //$bookWithIdArray = explode(':', $bookWithCategory[$i][1]);
			
            $bookWithIdArray = $bookWithCategory[$i][1];
			//
            $bookWithIdData = explode(',', $bookWithIdArray);

            $bookIds = array();
            foreach($bookWithIdData as $bookWithId)
            {
                $bookIdArray = explode('-', $bookWithId);
                $bookIds[] = $bookIdArray[0];
            }

            sort($bookIds);

            $bookWithIdArray = trim(implode(',', array_unique($bookIds)),",");

			//END
			
            if(!empty($bookWithIdArray)){
              
                $select = db_select('booksection', 'b');
                $select->fields('b', array('FundamentalworkId', 'ActualFundamentalwork', 'FundamentalworkImg', 'Title'));
                $select->fields('bd', array('LanguageID', 'author'));
                $select->fields('bl', array('language_code'));
                $select->fields('bc', array('BookCategoryId'));
                $select->Join('booksection_detail', 'bd', 'b.ActualFundamentalwork = bd.ActualFundamentalworkID');
                $select->Join('booksection_language', 'bl', 'b.LanguageID = bl.language_id');
                $select->leftJoin('booksection_categories', 'bc', 'b.FundamentalworkId = bc.fundamental_book_id');
                $select->condition('bc.BookCategoryId', $bookWithCategory[$i][0]);
                //$select->condition('bl.language_code', $language->language);
                $select->where('b.LanguageID = bd.LanguageID');
                $select->where('b.FundamentalworkId in ('.$bookWithIdArray.')');
                $select->groupby('b.FundamentalworkId');
                //$test = $select->execute()->fetchAll();
                
                
                $selectTotal = $select;
                $booksectionTotalCount = $booksectionTotalCount + $selectTotal->execute()->rowCount();
                $listbooksresult[$Cat_name.'-'.$bookWithCategory[$i][0]]['sectionTotalcount'] = $selectTotal->execute()->rowCount();
                $select = $select->extend('PagerDefault')->element('2'.$bookWithCategory[$i][0])->limit(3);
                $listbooksresult[$Cat_name.'-'.$bookWithCategory[$i][0]]['sectionItemListing'] = $select->execute()->fetchAll();
                
                 
              
            }
           
        }
        //print_r($listbooksresult); 
        $main_result['photoReferencesBooksection'] = $listbooksresult;
        $main_result['booksectionTotalCount'] = $booksectionTotalCount;

        $main_result['booksection_section_format'] = $photoReferencesbooksections[0]->section_format;
     
    }
    
    
//    // START FOR BREADCRUMB
//    $breadcrumb = array();
//    $breadcrumb[] = l(t('Home'), '<front>', array('attributes' => array('title' => t('Home'))));
//    $breadcrumb[] = l(t('Gallery'), 'gallery', array('attributes' => array('title' => t('Gallery'))));
//    $breadcrumb[] = l(t('Photos'), 'photos', array('attributes' => array('title' => t('Photos'))));
//    $breadcrumb[] = t('Photo Detail Page');
//    drupal_set_breadcrumb($breadcrumb);
//
//    drupal_set_title(t('Photo Detail'));

    // END FOR BREADCRUMB

    $videoCount = (isset($main_result['videoTotalCount']) && $main_result['videoTotalCount'] != '') ? $main_result['videoTotalCount'] : '0';
    $cartoonCount = (isset($main_result['cartoonTotalCount']) && $main_result['cartoonTotalCount'] != '') ? $main_result['cartoonTotalCount'] : '0';
    $audioCount = (isset($main_result['audioTotalCount']) && $main_result['audioTotalCount'] != '') ? $main_result['audioTotalCount'] : '0';
    $posterCount = (isset($main_result['posterTotalCount']) && $main_result['posterTotalCount'] != '') ? $main_result['posterTotalCount'] : '0';
    $stampCount = (isset($main_result['stampTotalCount']) && $main_result['stampTotalCount'] != '') ? $main_result['stampTotalCount'] : '0';
    $cwmgCount = (isset($main_result['cwmgTotalCount']) && $main_result['cwmgTotalCount'] != '') ? $main_result['cwmgTotalCount'] : '0';
    $tributeCount = (isset($main_result['tributeTotalCount']) && $main_result['tributeTotalCount'] != '') ? $main_result['tributeTotalCount'] : '0';
    $variorumCount = (isset($main_result['variorumTotalCount']) && $main_result['variorumTotalCount'] != '') ? $main_result['variorumTotalCount'] : '0';
    $bookCount = (isset($main_result['bookTotalCount']) && $main_result['bookTotalCount'] != '') ? $main_result['bookTotalCount'] : '0';
    $journalgandhiCount = (isset($main_result['journalgandhiTotalCount']) && $main_result['journalgandhiTotalCount'] != '') ? $main_result['journalgandhiTotalCount'] : '0';
    $journalotherCount = (isset($main_result['journalotherTotalCount']) && $main_result['journalotherTotalCount'] != '') ? $main_result['journalotherTotalCount'] : '0';
    $othersectionCount = (isset($main_result['booksectionTotalCount']) && $main_result['booksectionTotalCount'] != '') ? $main_result['booksectionTotalCount'] : '0';
	
    $output = '';

    /* Check Js is active or not in client browsers */
    $jsstatus = 0;
    if (isset($_POST['jsstatus'])) {
        if ($_POST['jsstatus'] == 1):
            $jsstatus = 1;
        endif;
    }

    $main_result['TabDisplayOrder'] = array('audio', 'poster', 'stamp', 'cwmg', 'tribute', 'video', 'cartoon', 'variorum', 'book', 'journalbygandhi', 'journalbyother','othersection');


    $output .= theme('ghp_tab_header_reference', array(
        'videoCount' => $videoCount,
        'cartoonCount' => $cartoonCount,
        'audioCount' => $audioCount,
        'posterCount' => $posterCount,
        'stampCount' => $stampCount,
        'variorumCount' => $variorumCount,
        'cwmgCount' => $cwmgCount,
        'tributeCount' => $tributeCount,
        'bookCount' => $bookCount,
        'othersectionCount' => $othersectionCount,
        'journalbygandhiCount' => $journalgandhiCount,
        'journalbyotherCount' => $journalotherCount,
        'activeTab' => $activeTab,
        'CurrentItemId' => $photoid,
        'jsstatus' => $jsstatus,
        'TabDisplayOrder' => $main_result['TabDisplayOrder']
            )
    );
   
    $output .= "<div id='refrence_details'><div class='refrence_details'>";
		
    if ($activeTab == 'video'):
        $output .= theme('ghp_video_reference', array(
            'ReferencesVideo' => $main_result['photoReferencesVideo'],
            'videoCount' => $main_result['videoTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'cartoon'):
        $output .= theme('ghp_cartoon_reference', array(
            'ReferencesCartoon' => $main_result['photoReferencesCartoon'],
            'cartoonCount' => $main_result['cartoonTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'audio'):
        $output .= theme('ghp_audio_reference', array(
            'ReferencesAudio' => $main_result['photoReferencesAudio'],
            'audioCount' => $main_result['audioTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'poster'):
        $output .= theme('ghp_poster_reference', array(
            'ReferencesPoster' => $main_result['photoReferencesPoster'],
            'posterCount' => $main_result['posterTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'stamp'):
        $output .= theme('ghp_stamp_reference', array(
            'ReferencesStamp' => $main_result['photoReferencesStamp'],
            'stampCount' => $main_result['stampTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'cwmg'):
        $output .= theme('ghp_cwmg_reference', array(
            'ReferencesCWMG' => $main_result['photoReferencesCWMG'],
            'cwmgCount' => $main_result['cwmgTotalCount'],
            'cwmgSections' => $main_result['cwmgSections'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'tribute'):
        $output .= theme('ghp_tribute_reference', array(
            'ReferencesTribute' => $main_result['photoReferencesTribute'],
            'tributeCount' => $main_result['tributeTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'variorum'):
        $output .= theme('ghp_variorum_reference', array(
            'ReferencesVariorum' => $main_result['photoReferencesVariorum'],
            'variorumCount' => $main_result['variorumTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
    elseif ($activeTab == 'book'):
  			$output .= theme('ghp_book_reference', array(
            'ReferencesBook' => $main_result['photoReferencesBook'],
            'bookCount' => $main_result['bookTotalCount'],
			'booksSections' => $main_result['booksSections'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        ); 
     elseif ($activeTab == 'journalbygandhi'):
     
     	     $output .= theme('ghp_journal_by_gandhi_reference', array(
	        'ReferencesJournalgandhi' => $main_result['photoReferencesJournalgandhi'],
	        'journalbygandhiCount' => $main_result['journalgandhiTotalCount'],
            'journalgandhi_section_format' => $main_result['journalgandhi_section_format'],
	        'activeTab' => $activeTab,
	        'jsstatus' => $jsstatus,
	        'CurrentItemId' => $photoid
	        )
	        
        );
    elseif ($activeTab == 'journalbyother'):
			$output .= theme('ghp_journal_by_other_reference', array(
            'ReferencesJournalother' => $main_result['photoReferencesJournalother'],
            'journalbyotherCount' => $main_result['journalotherTotalCount'],
            'journalother_section_format' => $main_result['journalother_section_format'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid
            )
       );    
    elseif ($activeTab == 'othersection'):
        $output .= theme('ghp_othersection_reference', array(
            'Referencesothersection' => $main_result['photoReferencesBooksection'],
            'othersectionCount' => $main_result['booksectionTotalCount'],
            'booksection_section_format' => $main_result['booksection_section_format'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );
	else :
        $output .= theme('ghp_video_reference', array(
            'ReferencesVideo' => $main_result['photoReferencesVideo'],
            'videoCount' => $main_result['videoTotalCount'],
            'activeTab' => $activeTab,
            'jsstatus' => $jsstatus,
            'CurrentItemId' => $photoid,
                )
        );

    endif;

    $output .= "</div></div>";

    $_SESSION['main_result'] = $main_result;
    //echo "<pre>";
    //print_r($_POST['photoSearchBtn']);
    //echo "</pre>";
    if ($jsstatus == 1 && !isset($_POST['photoSearchBtn'])) {

        print_r($output);
        exit;
    }

    return $output;
}

function get_ajax_photos_data() {

    global $language, $base_url, $photo_query_condition;

    $photoResultObject = array();

    // GET THE REFERENCE ARGUMENT
    $display_photos_of = arg(1);
    $display_photo_reference_id = arg(2);
    bindQueryCondition($display_photos_of, $display_photo_reference_id);
    // END CODE

    $FieldSelectedArray = unserialize(variable_get("ManageFieldSelectedArray"));
    // SET VARIABLE FROM GLOBAL SEARCH KEYWORD
    if (isset($_POST['global_search_keyword']) && $_POST['global_search_keyword'] != '') {
        $filtered_global_search = filtertext(strip_tags($_POST['global_search_keyword']));

        $global_search_condition = " AND ( " . implode(" LIKE '%" . $filtered_global_search . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $filtered_global_search . "%' ) ";

        //$global_search_condition = " AND ( pd.photo_title LIKE '%".$_POST['global_search_keyword']."%' OR pd.photo_description LIKE '%".$_POST['global_search_keyword']."%' ) ";
    } else {
        $global_search_condition = '';
    }

    // FILTER POSTED DATA
    $filtered_language_code = filtertext(strip_tags($_POST['language']));
    $filtered_page_number = (int) filtertext(strip_tags($_POST['page']));
    $filtered_category_id = filtertext(strip_tags($_POST['photo_category']));
    $filtered_photo_search_text = filtertext(strip_tags($_POST['photo_search_keyword']));

    $photoSearchCond = '';
    if ($filtered_photo_search_text != '') {
        $photoSearchCond .= " AND ( " . implode(" LIKE '%" . $filtered_photo_search_text . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $filtered_photo_search_text . "%' ) ";
    }

    if ($filtered_category_id != '') {
        $photoSearchCond .= " AND pm.category_id = '" . $filtered_category_id . "' ";
    }

    $mainSql = "SELECT pm.*,pd.*, YEAR(pm.photo_date) AS photoyear FROM photo_master pm 
        LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id 
        LEFT JOIN photo_category pc ON pm.category_id = pc.category_id 
        WHERE pd.language_code='" . $filtered_language_code . "' 
        AND pm.photo_publish_status = 'Y' 
        AND pm.photo_approve_status = 'Y' " . $photoSearchCond;

    $columnOrderSet = " ORDER BY pc.category_id ASC, pm.photo_order_number ASC ";

    if (isset($_POST['from_year']) && $_POST['from_year'] != '' && isset($_POST['to_year']) && $_POST['to_year'] != '' && !empty($_POST['language'])) {

        $mainSql .= "  AND YEAR(pm.photo_date) >=  '" . $_POST["from_year"] . "' AND YEAR(pm.photo_date) <=  '" . $_POST["to_year"] . "' " . $photo_query_condition . " " . $global_search_condition . $columnOrderSet;

        $photoRange = db_query($mainSql . " LIMIT 0,18");
        $photoCount = db_query($mainSql);

        $photoResultObject = $photoRange->fetchAll();
        $totalRecords = $photoCount->rowCount();

        if ($display_photos_of != '' && $display_photo_reference_id != '') {
            $photoResultObject[]->extra_url_response = $display_photos_of . "/" . $display_photo_reference_id;
        } else {
            $photoResultObject[]->extra_url_response = '';
        }

        $photoResultObject[]->total_records = $totalRecords;

        // header('Content-type: application/json');
        // echo json_encode($photoResultObject);
        // exit;
    }

    // ADD CONDITON TO GET RECORD BASED ON SELECTED CHECKBOXES
    if (isset($_POST['checkbox_filter']) && $_POST['checkbox_filter'] != '' && !empty($_POST['language'])) {

        $checkboxArray = explode("##", $_POST['selected_checkbox']);
        $total_selected_checkbox = count($checkboxArray);
        $selectedArray = array();
        for ($k = 0; $k < $total_selected_checkbox; $k++) {
            if ($checkboxArray[$k] != '') {
                list($from, $to) = explode("-", $checkboxArray[$k]);
                for ($x = $from; $x <= $to; $x++) {
                    $selectedArray[] = $x;
                }
            }
        }

        if (count($selectedArray) > 0) {

            $filterYear = implode(",", array_unique($selectedArray));

            $mainSql .= " AND YEAR(pm.photo_date) IN (" . $filterYear . ") " . $photo_query_condition . " " . $global_search_condition . $columnOrderSet;
            $photoRange = db_query($mainSql . " LIMIT 0,18");
            $photoCount = db_query($mainSql);

            $photoResultObject = $photoRange->fetchAll();
            $totalRecords = $photoCount->rowCount();

            if ($display_photos_of != '' && $display_photo_reference_id != '') {
                $photoResultObject[]->extra_url_response = $display_photos_of . "/" . $display_photo_reference_id;
            } else {
                $photoResultObject[]->extra_url_response = '';
            }
            $photoResultObject[]->total_records = $totalRecords;
        } else {
            $photoResultObject = '';
        }
        // header('Content-type: application/json');
        // echo json_encode($photoResultObject);
        // exit;
    }
    // END CONDITION     

    if (!empty($_POST['language']) && ($_POST['page'] != '')) {

        $mainSql .= $photo_query_condition . " " . $global_search_condition . $columnOrderSet;

        $photoRange = db_query($mainSql . " LIMIT " . $filtered_page_number . ",18");
        $photoCount = db_query($mainSql);

        $photoResultObject = $photoRange->fetchAll();
        $totalRecords = $photoCount->rowCount();

        if ($display_photos_of != '' && $display_photo_reference_id != '') {
            $photoResultObject[]->extra_url_response = $display_photos_of . "/" . $display_photo_reference_id;
        } else {
            $photoResultObject[]->extra_url_response = '';
        }
        $photoResultObject[]->total_records = $totalRecords;
        // header('Content-type: application/json');
        // echo json_encode($photoResultObject);
        // exit;
    }

    foreach($photoResultObject as &$photoRow)
    {
        $photo_filename = $photoRow->photo_filename;
        if(empty($photo_filename) )
        {
            $photoRow->photo_filename = 'photo-gallery-no-img.jpg';
        }
        else
        {
			

            $photo_small_url = variable_get('ghp_data_root_path') . 'ghp_photos/small/';
            $smallImage = $photo_small_url . $photoRow->photo_filename;
            if (!file_exists($smallImage)) {
                $photoRow->photo_filename = 'photo-gallery-no-img.jpg';
            }
        }
    }

    header('Content-type: application/json');
    echo json_encode($photoResultObject);
    exit;
}

function get_ajax_thumb_photos_data() {

    global $language, $base_path, $base_url, $photo_query_condition;

    // GET THE REFERENCE ARGUMENT
    $display_photos_of = arg(1);
    $display_photo_reference_id = arg(2);
    bindQueryCondition($display_photos_of, $display_photo_reference_id);
    // END CODE
    // FILTER POSTED DATA
    $filtered_language_code = filtertext(strip_tags($_POST['language']));
    $filtered_page_number = (int) filtertext(strip_tags($_POST['page']));

    // SET VARIABLE FROM GLOBAL SEARCH KEYWORD
    if (isset($_POST['global_search_keyword']) && $_POST['global_search_keyword'] != '') {
        $FieldSelectedArray = unserialize(variable_get("ManageFieldSelectedArray"));
        $filtered_global_search = filtertext(strip_tags($_POST['global_search_keyword']));

        $global_search_condition = " AND ( " . implode(" LIKE '%" . $filtered_global_search . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $filtered_global_search . "%' ) ";
    } else {
        $global_search_condition = '';
    }

    if (isset($_POST['from_year']) && $_POST['from_year'] != '' && isset($_POST['to_year']) && $_POST['to_year'] != '' && !empty($_POST['language'])) {

        $photoRange = db_query("select pm.*,pd.*, YEAR(pm.photo_date) as photoyear from photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' AND YEAR(pm.photo_date) >=  '" . $_POST["from_year"] . "' AND YEAR(pm.photo_date) <=  '" . $_POST["to_year"] . "'  " . $photo_query_condition . $global_search_condition . "  ORDER BY pm.photo_id ASC limit 0,15");

        $photoResultObject = $photoRange->fetchAll();
        header('Content-type: application/json');
        echo json_encode($photoResultObject);
        exit;
    }


    // ADD CONDITON TO GET RECORD BASED ON SELECTED CHECKBOXES

    if (isset($_POST['checkbox_filter']) && $_POST['checkbox_filter'] != '' && !empty($_POST['language'])) {
        $checkboxArray = explode("##", $_POST['selected_checkbox']);
        $total_selected_checkbox = count($checkboxArray);
        $selectedArray = array();
        for ($k = 0; $k < $total_selected_checkbox; $k++) {
            if ($checkboxArray[$k] != '') {
                list($from, $to) = explode("-", $checkboxArray[$k]);
                for ($x = $from; $x <= $to; $x++) {
                    $selectedArray[] = $x;
                }
            }
        }

        if (count($selectedArray) > 0) {
            $filterYear = implode(",", array_unique($selectedArray));

            $photoRange = db_query("select pm.*,pd.*, YEAR(pm.photo_date) as photoyear from photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' AND YEAR(pm.photo_date) IN (" . $filterYear . ") " . $global_search_condition . " ORDER BY pm.photo_id ASC limit 0,15");
        } else {

            $photoRange = db_query("select pm.*,pd.*, YEAR(pm.photo_date) as photoyear from photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' " . $global_search_condition . " ORDER BY pm.photo_id ASC limit 0,15");
        }

        $photoResultObject = $photoRange->fetchAll();
        header('Content-type: application/json');
        echo json_encode($photoResultObject);
        exit;
    }
    // END CONDITION     

    if (!empty($_POST['language']) && ($_POST['page'] != '')) {

        $photoRange = db_query("select pm.*,pd.*, YEAR(pm.photo_date) as photoyear from photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' ORDER BY pm.photo_id ASC LIMIT " . $filtered_page_number . ",10");

        $photoResultObject = $photoRange->fetchAll();

        header('Content-type: application/json');
        echo json_encode($photoResultObject);
        exit;
    }
}

function ghp_photos_all_thumb_ajaxphotos_list() {

    global $language, $base_path, $base_url, $photo_query_condition;

    // GET THE REFERENCE ARGUMENT
    $display_photos_of = arg(1);
    $display_photo_reference_id = arg(2);
    bindQueryCondition($display_photos_of, $display_photo_reference_id);
    // END CODE

    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/ghp_photos.js');

    // SET VARIABLE FROM GLOBAL SEARCH KEYWORD
    if (isset($_POST['global_search_keyword']) && $_POST['global_search_keyword'] != '') {
        $FieldSelectedArray = unserialize(variable_get("ManageFieldSelectedArray"));
        // FILTER POSTED DATA
        $filtered_global_search_keyword = filtertext(strip_tags($_POST['global_search_keyword']));
        $filtered_language_code = filtertext(strip_tags($_POST['language']));

        $global_search_condition = " AND ( " . implode(" LIKE '%" . $filtered_global_search_keyword . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $filtered_global_search_keyword . "%' ) ";
    } else {
        $global_search_condition = '';
    }

    if (isset($_POST['checkbox_filter']) && $_POST['checkbox_filter'] != '' && !empty($_POST['language'])) {
        $checkboxArray = explode("##", $_POST['selected_checkbox']);
        $total_selected_checkbox = count($checkboxArray);
        $selectedArray = array();
        for ($k = 0; $k < $total_selected_checkbox; $k++) {
            if ($checkboxArray[$k] != '') {
                list($from, $to) = explode("-", $checkboxArray[$k]);
                for ($x = $from; $x <= $to; $x++) {
                    $selectedArray[] = $x;
                }
            }
        }

        $filterYear = implode(",", array_unique($selectedArray));

        if (count($selectedArray) > 0) {

            $photoRange = db_query("select pm.*,pd.*, YEAR(pm.photo_date) as photoyear from photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' AND YEAR(pm.photo_date) IN (" . $filterYear . ")  " . $photo_query_condition . " " . $global_search_condition . "  ORDER BY pm.photo_id ASC limit 0,15");
        } else {

            $photoRange = db_query("select pm.*,pd.*, YEAR(pm.photo_date) as photoyear from photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id where pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y'  " . $photo_query_condition . " " . $global_search_condition . " ORDER BY pm.photo_id ASC limit 0,15");
        }

        $photoResultObject = $photoRange->fetchAll();
        header('Content-type: application/json');
        echo json_encode($photoResultObject);
        exit;
    }
}

/* Function to start photo gallery Thumbnail view */

function ghp_all_thumb_photos_list() {

    // START FOR BREADCRUMB
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), '<front>', array('attributes' => array('title' => t('Home'))));
    $breadcrumb[] = l(t('Gallery'), 'gallery', array('attributes' => array('title' => t('Gallery'))));
    $breadcrumb[] = t('Photos');

    drupal_set_title(t('Photos'));
    drupal_set_breadcrumb($breadcrumb);
    // END FOR BREADCRUMB

    global $language, $base_path, $base_url, $photo_query_condition;
    
    $ghp_data_root_path = variable_get('ghp_data_root_path');
    $get_photo_path = variable_get('ghp_photos_path');
		$ghp_photo_download_path = variable_get('ghp_photo_download_images_path');
    //drupal_add_js(drupal_get_path('module', 'ghp_photos').'/js/slide_photo.js');
    //drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery-1.4.3.min.js');
    //drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/icheck.js');
    drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/skins/square/grey.css');

    // GET THE REFERENCE ARGUMENT
    $display_photos_of = filtertext(strip_tags(arg(1)));

    if ($display_photos_of == 'ghs') {
        $cond_field_name = 'h_site_id';
    }
    $cond_fieldvalue = (int) (filtertext(strip_tags(base64_decode(arg(2)))));
    $display_photo_reference_id = arg(2);

    $seturl = '';
    if ($display_photos_of != '' && $display_photo_reference_id != '') {
        $seturl = $display_photos_of . "/" . $display_photo_reference_id;
    }

    $fieldname_to_condition = bindQueryCondition($display_photos_of, $display_photo_reference_id);
    // END CODE;
    $record_per_page = 15; // Added by Maya
    // GET PHOTO CATEGORY LIST
    $select = db_select('photo_category', 'c');
    $select->fields('c', array('category_id', 'category_order'));
    $select->fields('ct', array('category_name'));
    $select->condition('ct.language_code', $language->language);
    $select->leftJoin('photo_category_detail', 'ct', 'c.category_id = ct.category_id');
    $select->orderBy('category_order', 'ASC');
    $photoCategoryList = $select->execute()->fetchAll();

    $parameters = array();
    $FieldSelectedArray = unserialize(variable_get("ManageFieldSelectedArray"));

    drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/jquery.ad-gallery.css');
    drupal_add_css(drupal_get_path('module', 'cwmg_language') . '/css/thickbox.css');
    drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/tagging.css');
	drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/thickbox.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/ghp_photos.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/htmlentities.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.livequery.js');

    drupal_add_js(drupal_get_path('module', 'gandhi_heritage_site') . '/js/jquery.uniform.min.js');
    drupal_add_css(drupal_get_path('module', 'cwmg_language') . '/css/uniform.default.css');

    $photoImagePath = $base_url . "/" . drupal_get_path('module', 'ghp_photos') . '/images/';
    $photo_url = $get_photo_path;
    $photo_small_url = $get_photo_path . 'small/';
    $photo_medium_url = $get_photo_path . 'medium/';

    $loadImgPath = $base_url . '/' . drupal_get_path('module', 'ghp_tributes') . '/images/loading.gif';
    $photoKeyword = '';
	if (isset($_POST['photoResetBtn']) == 'Reset') {
    $photoKeyword1 = $_POST['photoKeyword1'] = '';
    $selectedCatID = $_POST['photo_category'] = '';
    drupal_goto("photos_thumbview"); 
	}
    if (isset($_POST['photoSearchBtn']))
    {
		if (!empty($_REQUEST['photoKeyword1'])) {
        $photoKeyword = (isset($_REQUEST['photoKeyword1']) ? filtertext(strip_tags($_REQUEST['photoKeyword1'])) : '');
       
    	}
    	if($_SESSION['photothumbModuleToken'] != $_POST['photothumbModuleToken'])
	    {
			form_set_error('photoKeyword1', t('Invalid parameter passed'));
			$photoKeyword = $_POST['photoKeyword'] = $_POST['photoKeyword1'] = '';
		}
    	if (hasHtml($_POST['photoKeyword1']))
    	{
            form_set_error('photoKeyword1', t('Please enter valid search keyword'));
            $photoKeyword = $_POST['photoKeyword'] = $_POST['photoKeyword1'] = '';
        }
    }    
	
    //if (isset($_POST['photoSearchBtn']) && (isset($_POST['photoKeyword']) && (trim($_POST['photoKeyword']) != '')))
    /*
      if (!empty($_REQUEST['photoKeyword']))
      {
      $photoKeyword = (isset($_REQUEST['photoKeyword']) ? filtertext(strip_tags($_REQUEST['photoKeyword'])) : '');
      }
     * 
     */
    
  

    /*if (isset($_REQUEST['photo_category'])) {
        $selected_category_id = filtertext(strip_tags($_REQUEST["photo_category"]));
        $selectedCatID = $selected_category_id;
        $_SESSION["selected_category_id"] = $selected_category_id;
    } elseif (isset($_SESSION["selected_category_id"])) {
        $selectedCatID = $_SESSION['selected_category_id'];
    } else {
        // unset($_SESSION["selected_category_id"]);
        $selectedCatID = '';
    }*/
    
    $photothumbModuleToken = md5(microtime());
	$_SESSION['photothumbModuleToken'] = $photothumbModuleToken;
	if (isset($_REQUEST['photo_category'])) {
        $selected_category_id = (int)filtertext(strip_tags($_REQUEST["photo_category"]));
        $selectedCatID = $selected_category_id;
        
    }


    if (isset($_POST['searchphotos']) && $_POST["searchphotos"] != '') {
        $view = 'slider';
        unset($_REQUEST['yearrangecheck']);
        unset($parameters['yearrangecheck']);
        unset($_REQUEST['view']);
    }

    if (isset($_POST['btnPhotoFilterYear']) && $_POST['btnPhotoFilterYear']) {
        $view = 'chkbox';
        unset($_REQUEST['startyear']);
        unset($_REQUEST['endyear']);
        unset($_REQUEST['view']);
        unset($parameters['startyear']);
        unset($parameters['endyear']);
    }

    if (isset($view)) {
        $parameters['view'] = $view;
    }

    if (isset($_REQUEST['view'])) {
        if ($_REQUEST['view'] == 'chkbox') {
            unset($_REQUEST['startyear']);
            unset($_REQUEST['endyear']);
            unset($parameters['startyear']);
            unset($parameters['endyear']);
        } elseif ($_REQUEST['view'] == 'slider') {
            unset($_REQUEST['yearrangecheck']);
            unset($parameters['yearrangecheck']);
        }
    }

    //$photoRange = db_select('photo_master', 'pm')->extend('PagerDefault')->limit(15);
    $photoRange = db_select('photo_master', 'pm');
    $photoRange->fields('pm');
    $photoRange->addExpression('YEAR(pm.photo_date)', 'photoyear');
    $photoRange->join('photo_detail', 'pd', 'pm.photo_id =  pd.photo_id');
    $photoRange->join('photo_category', 'pc', 'pm.category_id =  pc.category_id');
    $photoRange->fields('pd');
    $photoRange->condition('pd.language_code', $language->language);
    $photoRange->condition('pm.photo_publish_status', 'Y');
    $photoRange->condition('pm.photo_approve_status', 'Y');

    if (isset($_REQUEST['header_search_keyword']) && $_REQUEST['header_search_keyword'] != '') {
        $global_search_keyword_value = $_REQUEST['header_search_keyword'];

        $searchcondstring = implode(" LIKE '%" . $global_search_keyword_value . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $global_search_keyword_value . "%' ";
        $photoRange->where(" ( " . $searchcondstring . " ) ");
        /*
          $photoRange->where( " ( pd.photo_title LIKE '%".$_SESSION['header_search_keyword']."%' OR pd.photo_description LIKE '%".$_SESSION['header_search_keyword']."%' ) " );
         * 
         */
    } else {
        $global_search_keyword_value = '';
    }

    // START FOR SEARCH TOPIC
    //if (isset($_POST['photoSearchBtn']) && (isset($photoKeyword) && $photoKeyword != '')) {
    if (!empty($photoKeyword)) {
        /*
          $db_or = db_or();
          $db_or->condition('pd.photo_title', '%' . $photoKeyword . '%', 'LIKE');
          $db_or->condition('pd.photo_description', '%' . $photoKeyword . '%', 'LIKE');
          $photoRange->condition($db_or);
         */

        $photoSearchCondition = implode(" LIKE '%" . $photoKeyword . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $photoKeyword . "%' ";

        $photoRange->where(" ( " . $photoSearchCondition . " ) ");

        $parameters['photoKeyword1'] = $photoKeyword;
    }
    if (isset($selectedCatID) && $selectedCatID != '') {
        $photoRange->condition("pm.category_id", $selectedCatID);
        $parameters['photo_category'] = $selectedCatID;
    }
    // END FOR SEARCH TOPIC       

    if ($photo_query_condition != '') {
        $photoRange->condition('pm.' . $cond_field_name, $cond_fieldvalue);
    }

    //$photoRange->where('YEAR(pm.photo_date) IN ('.$filterYear.')');

    if (isset($_REQUEST['startyear']) && $_REQUEST['startyear'] != '' && isset($_REQUEST['endyear']) && $_REQUEST['endyear'] != '') {

        if (isset($_GET["startyear"])) {
            $_POST["startyear"] = $_GET["startyear"];
        }

        if (isset($_GET["endyear"])) {
            $_POST["endyear"] = $_GET["endyear"];
        }

        if ($_POST['startyear'] > $_POST['endyear']) {
            form_set_error('startyear', t('Start year should be less than End year.'));
        } else {
            $photoRange->where('YEAR(pm.photo_date) >= :startyear', array('startyear' => $_POST['startyear']));
            $photoRange->where('YEAR(pm.photo_date) <= :endyear', array('endyear' => $_POST['endyear']));

            $parameters['startyear'] = $_POST['startyear'];
            $parameters['endyear'] = $_POST['endyear'];
        }

        unset($_REQUEST['yearrangecheck']);
        unset($parameters['yearrangecheck']);
    }

    if (isset($_REQUEST['yearrangecheck'])) {
        if (isset($_GET["yearrangecheck"])) {
            $_POST["yearrangecheck"] = $_GET["yearrangecheck"];
        }

        $parameters['yearrangecheck'] = $_POST['yearrangecheck'];
        $total_selected_checkbox = count($_POST['yearrangecheck']);
        $selectedArray = array();
        for ($k = 0; $k < $total_selected_checkbox; $k++) {
            list($from, $to) = explode("-", $_POST['yearrangecheck'][$k]);
            for ($x = $from; $x <= $to; $x++) {
                $selectedArray[] = $x;
            }
        }

        $filterYear = implode(",", array_unique($selectedArray));
        if (count($selectedArray) > 0) {
            $filterYear = implode(",", array_unique($selectedArray));
            $photoRange->where('YEAR(pm.photo_date) IN (' . $filterYear . ')');
        }

        unset($_REQUEST['startyear']);
        unset($_REQUEST['endyear']);
        unset($parameters['startyear']);
        unset($parameters['endyear']);
    }
    $photoRange->orderBy('pc.category_id', 'ASC');
    $photoRange->orderBy('pm.photo_order_number ', 'ASC');

    /* ... get total record counter Added by Maya... */
    $countval = $photoRange->execute()->rowCount();
    $photoRange = $photoRange->extend('PagerDefault')->limit($record_per_page);

    $photodatarray = (array) $photoRange->execute()->fetchAll();
    $currentcount = count($photodatarray);

    /* ...  Fetch page no to calculate Change record count for disable js Added by Maya .... */
    $currentpage = 1;
    $startcount = 0;
    $lastpageid = intval($countval / $record_per_page) + 1;
    if (isset($_GET["page"]) && $_GET["page"] != '') {
        $currentpage = (intval($_GET["page"]) + 1);
        $startcount = intval($_GET["page"]) * $record_per_page;

        if ($currentpage <= $lastpageid) {
            $currentcount = intval($startcount) + $currentcount;
            $startcount = $startcount + 1;
        }
    }
    $i = 0;
//<span class='title'>" . $photodata->photo_title . "</span>
    $defaultlan = language_default();
    $trans_url = $base_url;
    if ($language->language != $defaultlan->language) {
        $trans_url .= "/" . $language->language;
    }
    $output = "<div id='GalleryLoaderDisplay' style='top: 261px; left: 659px;'><img src='" . $loadImgPath . "' alt='Loading' /></div><div class='photohead photoheadWrap'><div class='midHead'><div class='head'><form name='photoSearch' id='photoSearch' action='" . $trans_url . "/photos_thumbview' method='post'><div class='volume'>";
    $output .= t('Category') . "</div><div class='volumeCount volumeselectdefault volumeselect' style='margin-top: 0px;'>";
    $output .= "<select name='photo_category' class='volumelistcls' id='photo_category'><option value=''>" . t('Select Category') . "</option>";
    foreach ($photoCategoryList as $key => $category) {
        if (isset($selectedCatID) && $selectedCatID == $category->category_id) {
            $selected = "selected='selected'";
        } else {
            $selected = "";
        }

        $output .= "<option value='" . $category->category_id . "' " . $selected . ">" . $category->category_name . "</option>";
    }

    //<div id='photoThumbSearchBlockPhp' class='photoThumbSearchWcag'><label><input type='text' name='photoKeyword' id='photoKeyword' value='".$photoKeyword."' /></label></div>
    $defaultlan = language_default();
    $trans_url = $base_url;
    if ($language->language != $defaultlan->language) {
        $trans_url .= "/" . $language->language;
    }

    $output .= "</select></div><div class='volume'>" . t('Search Topic') . "</div><div id='photoThumbSearchBlock' class='cwmgSearch'><label><input type='text' name='photoKeyword1' id='photoKeyword1' value='" . $photoKeyword . "' /></label></div><div><input id='photothumbModuleToken' type='hidden' readonly='readonly' value=".$photothumbModuleToken." name='photothumbModuleToken'><input type='submit' name='photoSearchBtn' id='photoSearchBtn' value='" . t("Search") . "' title='" . t("Search") . "' class='cwmgBtn' />
	  <input type='submit' name='photoResetBtn' id='photoResetBtn' value=". t('Reset'). " title=". t("Reset") . " class='cwmgBtn' />
	</div></form>";
    $output .= "<div class='volume' id='withjs' style='display:none' >" . t('Showing') . ": <span id='currentcount'>" . $currentcount . "</span> / " . $countval . " " . t('Photos') . "</div>";
    //$output .= "<a id='downloadZip' href='' title='Download'><img src='".$base_url."/sites/all/themes/ghp/images/download-icon.png'></a>";
    $output .= "<div class='viewbox'>";
	//$output .= "<div class='downloadFeedSec'><a id='rssvolume' target='_blank' href='" . $trans_url . "/photos/rss' class='rssfeed newFeedSpacing' title='Rss'></a> </div>"; 
	$output .= "<ul><li><span>" . t('Standard View') . "</span><a href='" . $trans_url . "/photos/" . $seturl . "' class='listview'></a></li><li><span>" . t('Thumbnail View') . "</span><a href='" . $trans_url . "/photos_thumbview/" . $seturl . "' class='thumbview current'></a></li></ul></div></div></div></div>";

    $output .= "<form id='galleryDisplay' name='galleryDisplay' method='post' action='" . $trans_url . "/photoDownloadZip'><div class='GalleryWrapper photoheadWrap'><div class='downBtnRel'><div class='downAbsulute'><input class='barDownload' type='submit' name='submit' value='Download' title='".t('Download')."'><input type='submit' id='Deselect' name='Deselect' value='".t('Deselect')."' title='".t('Deselect')."'>
		</div></div>";

    if ($display_photos_of != '' && $display_photo_reference_id != '') {
        $output .= '<input type="hidden" name="display_photos_url" id="display_photos_url" value="' . $display_photos_of . "/" . $display_photo_reference_id . '" />';
    }


    $output .= "<ul id='updates' style='display:none'>";
    if (count($photodatarray) > 0) {
        foreach ($photodatarray as $photodata) {

            if ($photodata->photo_alter_text != '') {
                $alttext = $photodata->photo_alter_text;
            } else {
                $alttext = $photodata->photo_filename;
            }

            if ($photodata->photo_title != '') {
                $titletext = $photodata->photo_title;
            } else {
                $titletext = $photodata->photo_filename;
            }

            $photoid = $photodata->photo_id;
            $photo_image = $ghp_data_root_path . 'ghp_photos/medium/' . $photodata->photo_filename;
            //echo $photo_image; exit;
            if (file_exists($photo_image)) {
                $photoUrl = $photo_medium_url . $photodata->photo_filename;
            } else {
                $photoUrl = $photo_medium_url . 'photo-gallery-no-img.jpg';
               
            }

						if(file_exists($ghp_photo_download_path.$photodata->photo_filename)){
							$checkbox = "<input type='checkbox' id='photozip_" . $photoid . "' name='photozip[]' class='photoZip' value='" . $photoid . "'>";
						}
						else
						{ 
							 $checkbox = '';
						}
						
            $output .="<li id='photothumb_" . $photoid . "' class='message_box'>" . $checkbox . "<a rel='example_group' href='" . $photo_url .'print/8x11/'. $photodata->photo_filename . "' class='productImage pop' title='" . $titletext . "' img_id='".$photoid."'><img src='" . $photoUrl . "' title='" . $titletext . "' alt='" . $alttext . "' id='".$photodata->photo_id."' img_id='".$photoid."' height='135' width='129' /><div id='tagbox_thumb'></div></a>";
						$output .= "<div class = 'file_size'>";
						if(file_exists($ghp_photo_download_path.$photodata->photo_filename)){
							 $photo_file_size = @filesize($ghp_photo_download_path.$photodata->photo_filename);
							 $photo_size = round($photo_file_size / 1024);
							 $output .= "( ". t('File Size') . " : " .$photo_size. " KB )";
						 }
						else
						{ }
						 $output .= "</div>";
						 $output .= "<input type='hidden' class='pcounter' name='photocounter_" . $i . "' id= 'photocounter_" . $i . "' value='" . $i . "' />
						<input type='hidden' name='baseUrl' id='baseUrl' value='".$base_url."' />
						<input type='hidden' class='phototitle' name='phototitle_" . $photoid . "' id= 'phototitle_" . $photoid . "' value='" . stripslashes($photodata->photo_title) . "' />
						<input type='hidden' class='photodesc' name='photodesc_" . $photoid . "' id= 'photodesc_" . $photoid . "' value='" . stripslashes($photodata->photo_description) . "' />

						<input type='hidden' class='photoplace' name='photoplace_" . $photoid . "' id= 'photoplace_" . $photoid . "' value='" . stripslashes($photodata->photo_place) . "' />
						<input type='hidden' class='photopeople' name='photopeople_" . $photoid . "' id= 'photopeople_" . $photoid . "' value='" . stripslashes($photodata->photo_people) . "' />
						<input type='hidden' class='photosource' name='photosource_" . $photoid . "' id= 'photosource_" . $photoid . "' value='" . stripslashes($photodata->photo_source) . "' />
						</li>";
            $i++;
        }
    } else {
        $output .= "<li style='background: none; width: 994px; text-align: center; font-weight:bold; min-height: 530px; margin:0;'>" . t('No Record Found.') . "</li>";
    }
    $output .="</ul>";

    $output .= "<ul id='updates1'>";

    if (count($photodatarray) > 0) {
        foreach ($photodatarray as $photodata) {
            $photoid = $photodata->photo_id;
            //echo $trans_url; exit; 

            $photo_image = $ghp_data_root_path . 'ghp_photos/medium/' . $photodata->photo_filename;
            //echo $photo_image; exit;
            if (file_exists($photo_image)) {
                $photoUrl = $photo_medium_url . $photodata->photo_filename;
               
            } else {
                $photoUrl = $photo_medium_url . 'photo-gallery-no-img.jpg';
            }

						if(file_exists($ghp_photo_download_path.$photodata->photo_filename)){
								$checkbox = "<input type='checkbox' id='photozip_" . $photoid . "' name='photozip[]' class='photoZip' value='" . $photoid . "' >";
						}
						else {
								$checkbox = '';
						}
						
            $output .="<li id='photothumbs_" . $photoid . "' class='message_box'>" . $checkbox . "<a class='productImage pop'><img src='" . $photoUrl . "' title='" . $photodata->photo_title . "' alt='" . $photodata->photo_title . "' height='135' width='129' /></a>";
							$output .= "<div class = 'file_size'>";
							if(file_exists($ghp_photo_download_path.$photodata->photo_filename)){
								 $photo_file_size = @filesize($ghp_photo_download_path.$photodata->photo_filename);
								 $photo_size = round($photo_file_size / 1024);
								 $output .= "( ". t('File Size') . " : " .$photo_size. " KB )";
							} else {
							}
						 $output .= "</div>";
						$output .= "<span class='title' style='display:none;'>" . $photodata->photo_title . "</span></li>";
							//$i++;
        }
    } else {
        $output .= "<li style='background: none; width: 994px; text-align: center; font-weight:bold; min-height: 530px; margin:0; '>" . t('No Record Found.') . "</li>";
    }
    $output .="</ul>";
    $defaultlan = language_default();
    $modulepath = $get_photo_path;
    $photoImagePath = drupal_get_path('module', 'ghp_photos') . '/images/';
    $photoImageUrl = $base_url . "/" . drupal_get_path('module', 'ghp_photos') . '/images/';
    //$photoImageUrl = url(drupal_get_path('module', 'ghp_photos') . '/images/', array('absolute' => true));

    $output .="<div style='display:inline;'><input type='hidden' name='selected_checkbox' id='selected_checkbox' value=''/>";
    $output .= '<input type="hidden" name="global_search_keyword" id="global_search_keyword" value="' . $global_search_keyword_value . '" />';
    $output .="<input type='hidden' name='photoDisplayCounter' id='photoDisplayCounter' value='" . $i . "' />";
    $output .="<input type='hidden' name='photo_id' id='photo_id' value='" . (isset($photoid) ? $photoid : '') . "' />";

    $output .="<input type='hidden' name='photo_paging_number' id='photo_paging_number' value='15' />";

    $output .="<input type='hidden' name='photo_url' id='photo_url' value='" . $photo_url . "' />";
    $output .="<input type='hidden' name='photo_small_url' id='photo_small_url' value='" . $photo_small_url . "' />";
    $output .="<input type='hidden' name='photo_medium_url' id='photo_medium_url' value='" . $photo_medium_url . "' />";

    $output .="<input type='hidden' name='phototitle' id='phototitle' value='" . t('Title') . "' />";
    $output .="<input type='hidden' name='photodesc' id='photodesc' value='" . t('Description') . "' />";
    $output .="<input type='hidden' name='photoplace' id='photoplace' value='" . t('Place') . "' />";
    $output .="<input type='hidden' name='photopeople' id='photopeople' value='" . t('People') . "' />";
    $output .="<input type='hidden' name='photosource' id='photosource' value='" . t('Source') . "' />";

    $output .="<input type='hidden' name='photoImagePath' id='photoImagePath' value='" . $photoImagePath . "'/>";
    $output .="<input type='hidden' name='modulepathval' id='modulepathval' value='" . $modulepath . "'/>";
    $output .="<input type='hidden' name='basepathvaluethumb' id='basepathvaluethumb' value='" . $base_url . "'/>";
    $output .="<input type='hidden' name='languagevalue' id='languagevalue' value='" . $language->language . "'/>";
    $output .="<input type='hidden' name='norecord' id='norecord' value='" . t('No Record Found.') . "'/>";
    $output .= "<input type='hidden' name='defaultlanguage' id='defaultlanguage' value='" . $defaultlan->language . "'/></div>";
    $output .= "<div class='facebook_style' id='facebook_style' style='display:none;'><img src='" . $photoImageUrl . "loading.gif' alt='Loading' /></div>";
    //  }
    // }
    $output .= "</div></form>";
    $output .= '<div id="wcagpaging">';
    $output .= theme('pager', array('tags' => array(), 'parameters' => $parameters));
    /* ... Added by Maya... */

    $startcounter = intval($currentcount - $record_per_page);
    $output .= "<label id='withoutjs' style='display:block; float:left;' >" . t('Page') . ": " . $currentpage . " / " . $lastpageid . "<br />" . t('Showing') . ": <span id='currentcount1'>" . $startcount . " - " . $currentcount . "</span> / " . $countval . " " . t('Photos') . "</label>";
    /* ................. */
    $output .="</div>";
    $output .= "<noscript></noscript><script type='text/javascript'>jQuery(document).ready(function($){ $('#withoutjs').hide(); $('#withjs').show(); });</script>"; //added by Maya
    //$output .= "<noscript></noscript><script type='text/javascript'>jQuery(document).ready(function(){ jQuery('input').iCheck({checkboxClass: 'icheckbox_square-grey'});});</script>";

    $output .= '<div class="copyrightBottom">';
    $output .= '<div class="copyBotimg">';
    $output .= '<img src="' . $base_url . '/sites/all/themes/ghp/images/copyright-info.jpg" />';
    $output .= '</div>';
    //Copyright at detail(item) level					
    /* 	$copyright_detail = "SELECT cm.copyright_id, c.title, c.description FROM copyright c INNER JOIN copyright_module cm ON c.copyright_id=cm.copyright_id INNER JOIN photo_master pm ON c.copyright_id = pm.copyright_id ";
      $copyrights = db_query($copyright_detail)->fetchAll();

      if(!empty($copyrights))
      {
      //print_r($copyrights); exit;
      $title = $copyrights[0]->title;
      $description = $copyrights[0]->description;
      //$output .= $title;
      //$output .= "Copyright Information";
      $output .= $description;
      }
      else
      { */
    //Copyright at list(module) level
    $copyright_list = "SELECT cm.copyright_id, c.title, c.description FROM copyright c INNER JOIN copyright_module cm ON c.copyright_id=cm.copyright_id WHERE module_name = 'photos'";
    $copyrights = db_query($copyright_list)->fetchAll();

    if (!empty($copyrights)) {
        //print_r($copyrights); exit;
        $title = $copyrights[0]->title;
        $description = $copyrights[0]->description;
        //$output .= $title;
        //$output .= "Copyright Information";
        $output .= $description;
    } else {
        //Copyright from by default flag
        $copyright_default = "SELECT c.title, c.description FROM copyright c WHERE c.default_flag='1'";
        $copyrights = db_query($copyright_default)->fetchAll();

        //print_r($copyrights); exit;
        $title = $copyrights[0]->title;
        $description = $copyrights[0]->description;
        //$output .= $title;
        //$output .= "Copyright Information";
        $output .= $description;
    }
    //}


    $output .= "</div> ";

    return $output;
}

/* Function to end photo gallery Thumbnail view */

function photoDownloadZip() {
	
     if(isset($_POST['Deselect'])){
        drupal_goto("photos_thumbview");
    }
    
    $photo_ids = $filename_photo = $count = '';
    
    if (isset($_POST['photozip'])) {
        $photo_ids = $_POST['photozip'];
    }

    if (isset($_POST['photozip'])) {
        $count = count($_POST['photozip']);
    }

    if ($count == 0) {
        drupal_set_message('Please select at least 1 check box for download zip.');
        drupal_goto("photos_thumbview");
    } else if ($count > 10) {
        drupal_set_message('You can select only 10 check boxes for download zip.');
        drupal_goto("photos_thumbview");
    } else {
        //echo "<pre>"; print_r($photo_ids);
        //exit;
        for ($i = 0; $i < count($photo_ids); $i++) {
            $select = db_select('photo_master');
            $fields = array('photo_filename');
            $select->fields('photo_master', $fields);
            $select->condition('photo_id', $photo_ids[$i]);
            $result = $select->execute()->fetchAll();
            $filename_photo .= ',' . $result[0]->photo_filename;
        }
        $filename_photo = ltrim($filename_photo, ',');

      //  $downloadfile_photo = drupal_realpath(file_default_scheme() . '://') . '\ghp_photos\\';
      //$ghp_data_root_path = variable_get('ghp_data_root_path');
   		$downloadfile_photo =  variable_get('ghp_photo_download_images_path');	
			$files = explode(',', $filename_photo);
        //print_r($files); exit; 

        //$path = drupal_realpath(file_default_scheme() . '://') . '\download\\';
      	$ghp_data_root_path = variable_get('ghp_data_root_path');
	echo $path =  $ghp_data_root_path.'download/';		//custom path
        $zipname = "photo_" . time() . ".zip";
        $zip_path = $path.$zipname;
        $zip = new ZipArchive;
        $res = $zip->open($zip_path, ZipArchive::CREATE);

        foreach ($files as $file) {
            $zip->addFile($downloadfile_photo . $file, $file);
        }
        $zip->close();
        header('Content-Type: application/zip');
        header("Content-Disposition: attachment; filename=" . $zipname);
        //header('Content-Length: ' . filesize($zipname));
        //header("Location: ".$zipname);
        header("Pragma: no-cache");
        header("Expires: 0");
        readfile("$zip_path");
        @chmod($zip_path, '0777');
        unlink($zip_path);
        exit;
        //drupal_goto("photos_thumbview");
    }
}

function ghp_all_thumb_ajax_photos_list() {

    global $language, $base_path, $base_url;

    $result = array();
    $get_photo_path = variable_get('ghp_photos_path');
    $ghp_data_root_path = variable_get('ghp_data_root_path');
		$ghp_photo_download_path = variable_get('ghp_photo_download_images_path');
    $photo_url = $get_photo_path;
    $photo_medium_url = $get_photo_path . 'medium/';

    $outputajax = '';
    $photoImagePath = drupal_get_path('module', 'ghp_photos') . '/images/';

    $FieldSelectedArray = unserialize(variable_get("ManageFieldSelectedArray"));

    // SET VARIABLE FROM GLOBAL SEARCH KEYWORD
    if (isset($_POST['global_search_keyword']) && $_POST['global_search_keyword'] != '') {
        $filtered_global_search = filtertext(strip_tags($_POST['global_search_keyword']));

        $global_search_condition = " AND ( " . implode(" LIKE '%" . $filtered_global_search . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $filtered_global_search . "%' ) ";
    } else {
        $global_search_condition = '';
    }

    $filtered_language_code = strip_tags($_POST['languagevalue']);
    $filtered_lastmsg = (int) filtertext(strip_tags($_POST['lastmsg']));
    $filtered_category_id = filtertext(strip_tags($_POST['photo_category']));
    $filtered_photo_search_text = filtertext(strip_tags($_POST['photo_search_keyword']));

    $filtered_last_paging_number = (int) filtertext(strip_tags($_POST['last_paging_number']));

    if (isset($_POST['lastmsg']) && is_numeric($_POST['lastmsg'])) {

        $photoSearchCond = "";
        if ($filtered_photo_search_text != "") {
            $photoSearchCond .= " AND ( " . implode(" LIKE '%" . $filtered_photo_search_text . "%' OR ", $FieldSelectedArray['photo_fields']) . " LIKE '%" . $filtered_photo_search_text . "%' ) ";
        }

        if ($filtered_category_id != '') {
            $photoSearchCond .= " AND pm.category_id = '" . $filtered_category_id . "' ";
        }

        //pd.photo_id >'" . $filtered_lastmsg . "'

        $mainQuery = "SELECT pm.*,pd.*, YEAR(pm.photo_date) AS photoyear FROM photo_master pm LEFT JOIN photo_detail pd ON pm.photo_id = pd.photo_id LEFT JOIN photo_category pc ON pm.category_id = pc.category_id WHERE 1 AND pd.language_code='" . $filtered_language_code . "' AND pm.photo_publish_status = 'Y' AND pm.photo_approve_status = 'Y' " . $photoSearchCond;

        if (!empty($_POST['selected_checkbox'])) {
            $checkboxArray = explode("##", $_POST['selected_checkbox']);
            $total_selected_checkbox = count($checkboxArray);
            $selectedArray = array();
            for ($k = 0; $k < $total_selected_checkbox; $k++) {
                if ($checkboxArray[$k] != '') {
                    list($from, $to) = explode("-", $checkboxArray[$k]);
                    for ($x = $from; $x <= $to; $x++) {
                        $selectedArray[] = $x;
                    }
                }
            }
            $filterYear = implode(",", array_unique($selectedArray));

            $mainQuery .= " AND YEAR(pm.photo_date) IN (" . $filterYear . ") " . $global_search_condition;
        }

        $mainQuery .= " ORDER BY pc.category_id ASC, pm.photo_order_number ASC LIMIT " . $filtered_last_paging_number . ",15";

        $photoquery1 = db_query($mainQuery);

        $i = $_POST['photoDisplayCounter'];
        $numrows1 = $photoquery1->rowCount();

        $photoResultArray = $photoquery1->fetchAll();

        //For file exists condition
        for ($i = 0; $i < $numrows1; $i++) {
            $photo_image = $ghp_data_root_path . 'ghp_photos/medium/' . $photoResultArray[$i]->photo_filename;
            if (file_exists($photo_image)) {
                $photoResultArray[$i]->flag = '1';
            } else {
                $photoResultArray[$i]->photo_filename = 'photo-gallery-no-img.jpg';
                $photoResultArray[$i]->flag = '0';
            }
						
						if(file_exists($ghp_photo_download_path.$photoResultArray[$i]->photo_filename)){
								$photo_file_size = filesize($ghp_photo_download_path.$photoResultArray[$i]->photo_filename);
								$photoResultArray[$i]->filesize = round($photo_file_size / 1024);
						}
						else {
								$photoResultArray[$i]->filesize = '';
						}
        }

        $result['totalResultCount'] = $numrows1;
        $result['result_display_data'] = $photoResultArray;
        // echo "<pre>"; print_r($photoResultArray);
        header('Content-type: application/json');
        echo json_encode($result);
        exit;
    }
}
