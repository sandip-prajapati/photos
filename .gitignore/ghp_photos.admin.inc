<?php

function ghp_photos_entry_load($entry = array()) {
    if (!empty($_REQUEST['languagename'])) {
        $languageid = $_REQUEST['languagename'];
    } else {
        $languageid = "en";
    }

    //$select = db_select('photo_master', 'pm');
    $select = db_select('photo_master', 'pm')->extend('PagerDefault')->limit(50);
    $select->fields('pm', array('photo_id'));
    $select->fields('pd', array('photo_title','photo_description', 'language_code'));
    $select->fields('pm', array('photo_filename'));
    $select->fields('pm', array('photo_date'));
    //$select->fields('lan', array('name'));
    $select->leftJoin('photo_detail', 'pd', 'pm.photo_id = pd.photo_id');
    $select->leftJoin('languages', 'lan', 'pd.language_code = lan.language');
    $select->condition('pd.language_code', $languageid);
    $select->orderBy('photo_id', 'DESC');

    // Return the result in object format.
    return $select->execute()->fetchAll();
}

function ghp_photos_list() {

    global $user, $base_url;

    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/ghp_photos_list.js');

    $adminRenamePath = ((variable_get('rename_admin_path') != NULL) ? variable_get('rename_admin_path_value') : 'admin');

    if (!empty($_REQUEST['languagename'])) {
        $languageid = $_REQUEST['languagename'];
    } else {
        $languageid = "en";
    }
    $parameters = array('languagename' => $languageid);
    $output = '';

    $selectlanguage = db_select('languages', 'lang');
    $selectlanguage->fields('lang', array('name', 'language'));
    $selectlanguage->condition('enabled', 1);
    $languagearray = $selectlanguage->execute()->fetchAll();

    $output .= '<form name="frmlang" method="post" action="ghp_photos">';
    $output .= '<input type="hidden" name="languageId" id="languageId" value="' . ((isset($_REQUEST['languagename'])) ? $_REQUEST['languagename'] : 'en') . '" />';
    $output .= '<select name="languagename" id="languagename">';

    foreach ($languagearray as $key => $languageval) {

        if (isset($_REQUEST['languagename']) && ($languageval->language == $_REQUEST['languagename'])) {
            $selectedlang = "selected=selected";
        } else {
            $selectedlang = "";
        }

        $output .='<option value="' . $languageval->language . '" ' . $selectedlang . '  >' . $languageval->name . '</option>';
    }
    $output .= '</select>';
    $output .='<input type="submit" class="btnbg" name="languagechange" title="Go" value="Go" />';
    $output .= '</form>';


    // Get all entries in the ghp_photos table.
    if ($entries = ghp_photos_entry_load()) {

        $output .= '<form name="frmGHPPhotos" method="post" action="ghp_photos/delete">
                    <input type="hidden" name="del_id" id="del_id" value="" /><input type="hidden" name="lang_code" id="lang_code" value="' . ((isset($_REQUEST['languagename'])) ? $_REQUEST['languagename'] : 'en') . '" />';

        $rows = array();
        $i = 1;
        foreach ($entries as $key => $entry) {            
           // $entry->photo_description = stripslashes(htmlentities($entry->photo_description,3)); 
            $entry->photo_description = stripslashes($entry->photo_description); 
                
            if (user_access('Add/Edit Photos')) {
                $entry->edit_link = '<a href="ghp_photos/add/' . $entry->language_code . '/' . $entry->photo_id . '" title="Edit Photo">Edit</a> ';
            }
            if (user_access('Delete Photos')) {
                $entry->delete_link = '<a href="#" onclick=delete_photo("' . $entry->photo_id . '","' . $entry->language_code . '") title="Delete Photo">Delete</a>';
            }

            if ((user_access('Approve Photos')) || (user_access('Publish Photos'))) {
                $entry->photo_id = '<input type="checkbox" class="selectbookcheck" value="' . $entry->photo_id . '" name="photoselect[]" id="checkbox' . $i . '" /> ';
            } else {
                unset($entry->photo_id);
            }

            $rows[$key] = (array) $entry;
            unset($rows[$key]['language_code']);
            $i++;
        }

        $header = array();
        if ((user_access('Approve Photos')) || (user_access('Publish Photos'))) {
            $header[] = '<input type="checkbox" name="photocheckall" onclick="toggleCheckboxes()" id="photocheckall"  />';
        }
        $header [] = '<strong>' . t('Photo Name') . '</strong>';
				$header [] = '<strong>' . t('Photo Description') . '</strong>';
        $header [] = '<strong>' . t('File Name') . '</strong>';
        $header [] = '<strong>' . t('Photo Date') . '</strong>';
        
        if (user_access('Add/Edit Photos'))
            $header [] = '<strong>' . t('Edit') . '</strong>';

        if (user_access('Delete Photos'))
            $header [] = '<strong>' . t('Delete') . '</strong>';

        if (in_array("administrator", $user->roles))
            $table_attributes = array('id' => 'tableStructure1');
        else
            $table_attributes = array('id' => 'tableStructure');

        $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => $table_attributes));
        $output .= theme('pager', array('tags' => array(), 'parameters' => $parameters));
        if (user_access('Publish Photos'))
        {
            $output .='<input type="button" class="btnbg" name="publishphoto" onclick="return photolistform(this.value);" value="Publish" />';
            $output .='<input type="button" class="btnbg" name="unpublishphoto" onclick="return photolistform(this.value);" value="Unpublish" />';
            
        }   
      
        if (user_access('Approve Photos'))
        {
            $output .='<input type="button" class="btnbg" name="approvephoto" onclick="return photolistform(this.value);" value="Approve" />';
            $output .='<input type="button" class="btnbg" name="rejectphoto" onclick="return photolistform(this.value);" value="Reject" />';
        }
        if (user_access('Delete Photos'))
        {
            $output .='<input type="button" class="btnbg" name="deletephotos" onclick="return photolistform(\'Deleteall\');" value="Delete" />';
        }
        $output .='<input type="hidden" name="baseurlpath" id="baseurlpath" value="' . $base_url . '" />';
        $output .='<input type="hidden" name="adminRenamePath" id="adminRenamePath" value="' . $adminRenamePath . '" />';
        $output .= '</form>';
    } else {
        drupal_set_message(t('No entries have been added yet.'));
    }
    return $output;
}

function ghp_photos_publish() {
    global $user;

    if (!empty($_POST['photoselect'])) {
        $photoids = implode(",", $_POST['photoselect']);
        $sqlupdate = "UPDATE photo_master SET photo_publish_status='Y',published_by=" . $user->uid . " where photo_id IN (" . $photoids . ")";
        db_query($sqlupdate);              
        
    }
    
    drupal_set_message(t('Photo Published Successfully.'));    
    if(!empty($_POST['lang_code'])){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $_POST['lang_code'],
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function ghp_photos_approve() {
    global $user;

    if (!empty($_POST['photoselect'])) {
        $photoids = implode(",", $_POST['photoselect']);
        $sqlupdate = "UPDATE photo_master SET photo_approve_status='Y',approved_by=" . $user->uid . " where photo_id IN (" . $photoids . ")";
        db_query($sqlupdate);
    }
    drupal_set_message(t('Photo Approved Successfully.'));
    if(!empty($_POST['lang_code'])){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $_POST['lang_code'],
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function ghp_photos_unpublish() {
    global $user;

    if (!empty($_POST['photoselect'])) {
              
        $photoids = implode(",", $_POST['photoselect']);                
        $sqlupdate = "UPDATE photo_master SET photo_publish_status='N',published_by=" . $user->uid . " where photo_id IN (" . $photoids . ")";
        db_query($sqlupdate);
    }
    
    drupal_set_message(t('Photo Unpublished Successfully.'));
    if(!empty($_POST['lang_code'])){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $_POST['lang_code'],
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function ghp_photos_reject() {
    global $user;

    if (!empty($_POST['photoselect'])) {
        $photoids = implode(",", $_POST['photoselect']);
               
        $queryphotoadmin = "SELECT DISTINCT(u.mail) FROM photo_master AS pm INNER JOIN users AS u ON (pm.photo_created_by = u.uid)
            INNER JOIN users_roles AS ur ON (u.uid = ur.uid)INNER JOIN role AS rl ON ( ur.rid = rl.rid )
            WHERE rl.name = 'Contributor'
            AND pm.photo_id
            IN (".$photoids.")";
       
       $resultphotoadmin = db_query($queryphotoadmin);    
       $rowphotoadmin = $resultphotoadmin->fetchAll();
       $begintext = t("Dear Friend,"); 
       $adminmsg = "The content submitted by you has not been found suitable for publication on the GHP. We hope that you would continue to send content suggestions to us.";
       $adminbodyhtml = recivecontentadminmail($begintext,$adminmsg);
       
       if(!empty($rowphotoadmin))
       {
           $adminusers = array();
           foreach($rowphotoadmin as $rowadmin)
           {
              $adminusers[] =  $rowadmin->mail;               
           }                
           
          $mailSendStatus = sendMail('GHP Contributor', $adminusers, t("Gandhi Heritage Portal: Content Rejected"), $adminbodyhtml);
       }
       
        
        $sqlupdate = "UPDATE photo_master SET photo_approve_status='N',approved_by=" . $user->uid . " where photo_id IN (" . $photoids . ")";
        db_query($sqlupdate);
    }
    drupal_set_message(t('Photo Rejected Successfully.'));
    if(!empty($_POST['lang_code'])){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $_POST['lang_code'],
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function get_photo_other_referenc_exist($photoId,$section_name){
    $photoDetailQuery = "select ref_id from photo_references where ref_id = '" . $photoId . "' and section_name='".$section_name."'";
    $photoDetailResultQuery = db_query($photoDetailQuery);
    $countPhotoDetail = $photoDetailResultQuery->rowCount();
    return $countPhotoDetail;
}
function get_photo_detail_exist($lang_code, $photoId) {

    // Check for Data Available in Photo Detail table or not
    $photoDetailQuery = "select photo_detail_id from photo_detail where language_code =  '" . $lang_code . "' AND photo_id = '" . $photoId . "'";
    $photoDetailResultQuery = db_query($photoDetailQuery);
    $countPhotoDetail = $photoDetailResultQuery->rowCount();
    return $countPhotoDetail;
}

function get_photo_data($lang_code, $photoId) {

    // Check for Data Available in Photo Detail table or not
    $countPhotoDetail = get_photo_detail_exist($lang_code, $photoId);

    if ($countPhotoDetail > 0) {
        $select = db_select('photo_master', 'pm');
        $select->fields('pm', array('photo_id', 'category_id', 'h_site_id', 'copyright_id','photo_order_number', 'photo_filename', 'photo_date','photo_display_date', 'uploaded_by', 'uploaded_date', 'modified_date', 'photo_created_by', 'photo_publish_status', 'photo_approve_status', 'photo_latitude', 'photo_longitude'));
        $select->fields('pd', array('language_code', 'photo_title', 'photo_alter_text', 'photo_description', 'photo_place','photo_source', 'photo_people', 'photo_significance', 'photographer_name', 'photo_meta_keywords', 'photo_meta_description', 'events', 'type_of_photograph'));
        $select->leftJoin('photo_detail', 'pd', 'pm.photo_id = pd.photo_id');
        $select->condition('language_code', $lang_code);
        $select->condition('pm.photo_id', $photoId);
        $results = $select->execute()->fetchAssoc();
    } else {
        $select = db_select('photo_master', 'pm');
        $select->fields('pm', array('photo_id', 'category_id', 'h_site_id', 'copyright_id', 'photo_order_number', 'photo_filename', 'photo_date', 'photo_display_date', 'uploaded_by', 'uploaded_date', 'modified_date', 'photo_created_by', 'photo_publish_status', 'photo_approve_status', 'photo_latitude', 'photo_longitude'));
        $select->condition('pm.photo_id', $photoId);
        $results = $select->execute()->fetchAssoc();
    }
    return $results;
}

function get_tag_data($photoId) {

    $select = db_select('photo_tags', 'pt');
    $select->fields('pt', array('tag_name'));
    $select->condition('pt.photo_id', $photoId);
    $results = $select->execute()->fetchAll();

    return $results;
}

function get_categories() {

    $categories = array('' => t('Select'));

    $select = db_select('category', 'c');
    $select->fields('c', array('category_id'));
    $select->fields('ct', array('category_name'));
    $select->condition('ct.language_code', 'en');
    $select->leftJoin('category_translation', 'ct', 'c.category_id = ct.category_id');
    $select->orderBy('c_translation_id', 'ASC');
    $results = $select->execute()->fetchAll();

    foreach ($results as $result) {
        $categories[$result->category_id] = $result->category_name;
    }

    return $categories;
}

function get_photo_categories() {

    $categories = array('' => t('Select'));

    $select = db_select('photo_category', 'c');
    $select->fields('c', array('category_id'));
    $select->fields('ct', array('category_name'));
    $select->condition('ct.language_code', 'en');
    $select->leftJoin('photo_category_detail', 'ct', 'c.category_id = ct.category_id');
    $select->orderBy('c.category_id', 'ASC');
    $results = $select->execute()->fetchAll();

    foreach ($results as $result) {
        $categories[$result->category_id] = $result->category_name;
    }

    return $categories;
}

function get_all_site_data($lang_code) {

    $select = db_select('heritage_sites', 'hsi');
    $select->fields('hsi', array('h_site_id'));
    $select->fields('hsit', array('site_name'));
    $select->fields('hct', array('country_name'));
    $select->fields('hst', array('state_name'));
    $select->leftJoin('heritage_sites_translation', 'hsit', 'hsi.h_site_id = hsit.h_site_id');
    $select->leftJoin('heritage_country_translation', 'hct', 'hsi.h_country_id = hct.h_country_id');
    $select->leftJoin('heritage_state_translation', 'hst', 'hsi.h_state_id = hst.h_state_id');
    $select->condition('hsit.language_code', $lang_code);
    $select->condition('hct.language_code', $lang_code);
    $select->condition('hst.language_code', $lang_code);
    $select->orderBy('hsit.site_name');
    $results = $select->execute()->fetchAll();

    return $results;
}

function copyright_entry_load_by_id() {

    $select_copyright = db_select('copyright');
    $fields_copyright = array('copyright_id', 'title', 'description');
    $select_copyright->fields('copyright', $fields_copyright);
    //$select->condition('status', '1');

    // Return the result in object format.
    $copyright_data = $select_copyright->execute()->fetchAll();

    $rows_copyright = array();

    $rows_copyright[0] = "Select Copyright";
    foreach ($copyright_data as $entry_copyright) {

        $rows_copyright[$entry_copyright->copyright_id] = $entry_copyright->title;
    }
    return $rows_copyright;
}

function getReferenceItem($refId) {
    $select = db_select('photo_references', 'pr');
    $select->fields('pr', array('id', 'ref_id', 'module_name', 'section_name', 'section_format'));
    $select->condition('ref_id', $refId);
    $select->orderBy('id', 'ASC');
    return $select->execute()->fetchAll();
}
function savetag()
{
	
if( !empty( $_POST['type'] ) && $_POST['type'] == "insert" )
{
 $id = $_POST['pic_id'];  
  $imgtype = $_POST['imgtype'];  
  $name = $_POST['name'];
  $pic_x = $_POST['pic_x'];
  $pic_y = $_POST['pic_y'];
  $sql = "INSERT INTO photo_tag (pic_id,imgtype,name,pic_x,pic_y) VALUES ( $id,'$imgtype', '$name', $pic_x, $pic_y )";
  //$qry = mysql_query($sql);
  $qry = db_query($sql);
}

if( !empty( $_POST['type'] ) && $_POST['type'] == "remove")
{
  $tag_id = $_POST['tag_id'];
  $imgtype = $_POST['imgtype'];
  $sql = "DELETE FROM photo_tag WHERE id = '".$tag_id."' and imgtype = '".$imgtype."'";
  $qry = db_query($sql);
}
}

function taglist()
{
//print_r($qry);
$data['boxes'] = '';
$datalarge['boxeslarge'] = '';
$data['lists'] = '';
$datalarge['listslarge'] = '';
if($_POST['imgtype'] == 'normal')
{
//$sql = "SELECT * FROM photo_tag WHERE pic_id= '".$_POST[ 'pic_id' ]."' and imgtype = '".$_POST['imgtype']."' " ;
$sql = "SELECT * FROM photo_tag WHERE pic_id= '".$_POST[ 'pic_id' ]."' and imgtype = 'normal' " ;
$qry = db_query($sql)->fetchAll();
foreach ($qry as $qry) {
	
    $data['boxes'] .= '<div class="tagview personbackend" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y . 'px;" id="view_'.$qry->id.'">';
	$data['boxes'] .= '<div class="person personbackend" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y  . 'px;">' . $qry->name . '</div>';
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
	
    $datalarge['boxeslarge'] .= '<div class="tagviewlarge personbackend" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y . 'px;" id="view_'.$qry->id.'">';
	$datalarge['boxeslarge'] .= '<div class="person personbackend" style="left:' . $qry->pic_x . 'px;top:' . $qry->pic_y  . 'px;">' . $qry->name . '</div>';
	$datalarge['boxeslarge'] .= '<div class="square"></div>';
	$datalarge['boxeslarge'] .= '</div>';
	
	$datalarge['listslarge'] .= '<li id="'.$qry->id.'" imgtype="'.$qry->imgtype.'"><a>' . $qry->name . '</a> (<a class="remove">Remove</a>)</li>';

    } 
	echo json_encode( $datalarge );    

}
	



}

function ghp_photo_add_form() {
    global $base_url, $user;
    
    

    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/ghp_photos_addedit.js');
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photo_taging.js');
    drupal_add_js(drupal_get_path('module', 'volume_import') . '/js/jquery.datepick.js');
    drupal_add_css(drupal_get_path('module', 'volume_import') . '/css/jquery.datepick.css');
    drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/tagging.css');

    $adminRenamePath = ((variable_get('rename_admin_path') != NULL) ? variable_get('rename_admin_path_value') : 'admin');

    $lang_code = arg(3);
    $photoId = arg(4);
    //$path = variable_get('ghp_data_root_path').'ghp_photos';
    $path = variable_get('ghp_photos_path');
    if (!empty($lang_code) && !empty($photoId)) {
        // GET DATA FROM LANGUAGE CODE & PHOTO ID
        $photoDataArray = get_photo_data($lang_code, $photoId);

        // GET TAG DATA
        $photoTagDataObject = get_tag_data($photoId);

        $photoTagData = '';
        foreach ($photoTagDataObject as $photoTagDataObjects) {
            $photoTagData .= $photoTagDataObjects->tag_name . ',';
        }
        $photoTagData = substr($photoTagData, 0, -1);

        $download = $base_url . "/" . $adminRenamePath . "/ghp_photos/ghpPhotoAdminDownload/" . $photoId;

        $downloadLink = '<a href="javascript:" onclick="window.location.href = \'' . $download . '\'; return false;" title="Download Image">Download</a>';
        
    } elseif (!empty($lang_code) && empty($photoId)) {
        $lang_code = $lang_code;
    } else {
        $lang_code = 'en';
    }

	//For Other References
	$referenceItem = getReferenceItem($photoId);
    $photo_id = '';
    $video_id = '';
    $cartoon_id = '';
    $audio_id = '';
    

    foreach ($referenceItem as $ref) {
        /*if ($ref->section_name == 'Photo') {
            $photo_id = $ref->section_format;
        }*/
        if ($ref->section_name == 'Video') {
            $video_id = $ref->section_format;
        }
        if ($ref->section_name == 'Cartoon') {
            $cartoon_id = $ref->section_format;
        }
        if ($ref->section_name == 'Audio') {
            $audio_id = $ref->section_format;
        }
        if ($ref->section_name == 'Poster') {
            $poster_id = $ref->section_format;
        }
        if ($ref->section_name == 'Stamp') {
            $stamp_id = $ref->section_format;
        }
        if ($ref->section_name == 'Variorum') {
            $variorum_id = $ref->section_format;
        }
        if ($ref->section_name == 'GHS') {
            $ghs_id = $ref->section_format;
        }
        if ($ref->section_name == 'DayToDayChronology') {
            $dtodchronology_id = $ref->section_format;
        }
        if ($ref->section_name == 'EventChronology') {
            $eventchronology_id = $ref->section_format;
        }
        if ($ref->section_name == 'Volume') {
            $collectedwork_id = $ref->section_format;
        }
        if ($ref->section_name == 'Book') {
            $keytext_id = $ref->section_format;
        }
        if ($ref->section_name == 'JournalByGandhiji') {
            $journalbygandhiji_id = $ref->section_format;
        }
        if ($ref->section_name == 'JournalByOthers') {
            $others_id = $ref->section_format;
        }
        if ($ref->section_name == 'Tribute') {
            $tributes_id = $ref->section_format;
        }
        if ($ref->section_name == 'FundamentalWork') {
            $fundamentals_id = $ref->section_format;
        }
        /*if ($ref->section_name == 'OtherBooks') {
            $otherbooks_id = $ref->section_format;
        }*/
    }
	
    $form = array('#attributes' => array('enctype' => "multipart/form-data", "class" => "ghpForm"));

    // GET LANGUAGES
    $languageArray = get_languages();

    // GET CATEGORIES
    $categoryArray = get_photo_categories();

    // GET HERITAGE SITES
    $siteAllArray = get_all_site_data('en');

    $siteArray = array(0 => t('Select'));

    foreach ($siteAllArray as $sitesArray) {
        $siteArray[$sitesArray->h_site_id] = $sitesArray->site_name . ' - ' . $sitesArray->state_name . ',' . $sitesArray->country_name;
    }

    $imagePath = base_path() . 'sites/all/modules/volume_import/images/';
	$options_copyright = copyright_entry_load_by_id();
    $form['baseUrl'] = array(
        '#type' => 'hidden',
        '#default_value' => $base_url . '/' . $adminRenamePath . '/ghp_photos/add/',
        '#attributes' => array('id' => 'baseUrl')
    );
    $form['baseUrlpath'] = array(
        '#type' => 'hidden',
        '#default_value' => $base_url . '/' . $adminRenamePath.'/',
        '#attributes' => array('id' => 'baseUrlpath')
    );

    $form['photoId'] = array(
        '#type' => 'hidden',
        '#default_value' => $photoId,
        '#attributes' => array('id' => 'photoId')
    );

    $form['photo_filename_hidden'] = array(
        '#type' => 'hidden',
        '#default_value' => ((isset($photoDataArray['photo_filename'])) ? $photoDataArray['photo_filename'] : ''),
        '#attributes' => array('id' => 'photo_filename_hidden')
    );

    $form['imagepath'] = array(
        '#type' => 'hidden',
        '#default_value' => $imagePath,
        '#attributes' => array('id' => 'imagepath')
    );

    $form['language'] = array('#type' => 'select',
        '#title' => t('Language'),
        '#options' => $languageArray,
        '#default_value' => $lang_code,
    );

    $form['category_id'] = array('#type' => 'select',
        '#title' => t('Category'),
        '#options' => $categoryArray,
        '#default_value' => ((isset($photoDataArray['category_id'])) ? $photoDataArray['category_id'] : ''),
        '#required' => TRUE,
    );

    $form['type_of_photograph'] = array('#type' => 'select',
        '#title' => t('Type of Photograph'),
        '#options' => array('sculptures' => 'Sculptures', 'ghs_photos' => 'GHS Photos'),
        '#default_value' => ((isset($photoDataArray['type_of_photograph'])) ? $photoDataArray['type_of_photograph'] : 'ghs_photos'),
        '#required' => FALSE,
    );
	
	$form['copyright_id'] = array('#type' => 'select',
        '#title' => t('Copyright'),
        '#options' => $options_copyright,
		 '#default_value' => ((isset($photoDataArray['copyright_id'])) ? $photoDataArray['copyright_id'] : '')
	);

    $form['events'] = array(
        '#type' => 'textfield',
        '#title' => t('Events'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['events'])) ? $photoDataArray['events'] : ''),
    );

    $form['h_site_id'] = array('#type' => 'select',
        '#title' => t('Heritage Site Name'),
        '#options' => $siteArray,
        '#default_value' => ((isset($photoDataArray['h_site_id'])) ? $photoDataArray['h_site_id'] : ''),
    );
    
    $form['photo_order_number'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Order Number'),
        '#size' => 5,
        '#maxlength' => 5,
        '#required' => TRUE,
        '#default_value' => ((isset($photoDataArray['photo_order_number'])) ? $photoDataArray['photo_order_number'] : ''),
    );

    $form['photo_title'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Name'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_title'])) ? stripslashes($photoDataArray['photo_title']) : ''),
    );

    $form['photo_description'] = array(
        //'#type' => 'text_format',
        //'#format'=>'full_html',
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#cols' => 10,
        '#rows' => 5,
        '#required' => FALSE,
        "#resizable" => FALSE,
        '#maxlength' => 1000,
        '#default_value' => ((isset($photoDataArray['photo_description'])) ? stripslashes($photoDataArray['photo_description']) : ''),
    );

    $form['photo_filename'] = array(
        '#type' => 'file',
        '#title' => t('Upload Photo') . ' <span title="This field is required." class="form-required"></span>',
        '#description' => t('Allowed file .jpg, .jpeg, .gif, .png only.'),
        '#size' => 20,
        '#required' => FALSE,
        '#prefix' => ((!empty($photoDataArray['photo_filename'])) ? '<div class="downloadFile">' . $downloadLink . '</div>' : ''),
    );
    if($photoDataArray['photo_filename'])
    {
		//logic of image resize
				list($imgwidth, $imgheight, $type, $attr) = getimagesize($path.$photoDataArray['photo_filename']); 
				$containerWidth =  524;
				$containerHeight =  406;
				$containerRatio = 1.2906403940887;
				$containerRatio = $containerWidth / $containerHeight;
				$imageRatio = ($imgwidth/$imgheight);

				if($imageRatio > $containerRatio){
					$width = ($containerWidth);
                    $height = ($containerWidth *(1/ $imageRatio)) ;
                }
                else if($imageRatio < $containerRatio){
                	
                    $width = ($containerHeight * $imageRatio);
                    $height = ($containerHeight) ;
                }
                else if($imageRatio == $containerRatio)
                {
                    if($imgwidth <= $imgheight){
                        $width = ($containerWidth);
                        $height = ($containerWidth *(1/ $imageRatio)) ;
                    }
                    else{
                        $width = ($containerHeight * $imageRatio);
                        $height = ($containerHeight);
                    }
                }
                $ghp_data_root_path = variable_get('ghp_data_root_path');
                $photo = $ghp_data_root_path . 'ghp_photos/' .$photoDataArray['photo_filename'];
                if(file_exists($photo)) 
                {
                $photo =  $path . $photoDataArray['photo_filename'];	
                $imgtags = "imgtags";
				$imgtaglarge = "imgtaglarge";
				}
				else
				{
				$photo =  $path . "photo-gallery-no-img.jpg";	
				$width = 129;
				$height = 135;
				$style= "margin: 120px; cursor:auto;";
				$imgtag = "";
				$imgtaglarge = "";
				}
	 			
  $form['photodisplay'] = array(
  '#prefix' => '<h3>Normal Image</h3><div id="container">
  				<div id="'.$imgtags.'"><img id="'.$photoDataArray['photo_id'].'" type="normal" name="'.$photoDataArray['photo_filename'].'" src="' . $photo . '" height="'.$height.'px" width="'.$width.'px" style="'.$style.'" />',
  '#suffix' => '<div id="tagbox"></div>
  				</div>
  				<div id="taglist"> 
			    <ol> 
			    </ol>
			  	</div>
				<div class="clear"></div>
				</div>
			  	<img class="ad-loaderback" src="'.$base_url.'/sites/all/modules/ghp_photos/images/loading.gif" alt="Loader" style="display: none;">
			  	<br><br> ' 
  );
  
  
  $form['photodisplaylarge'] = array(
  '#prefix' => '<h3>Large Image</h3><div id="container">
  				<div id="'.$imgtaglarge.'"><img id="'.$photoDataArray['photo_id'].'" type="large" name="'.$photoDataArray['photo_filename'].'" src="' . $photo . '" />',
  '#suffix' => '<div id="tagboxlarge"></div>
  				</div>
  				<div id="taglistlarge"> 
			    <ol> 
			    </ol>
			  	</div>
				<div class="clear"></div>
				</div>
			  	<img class="ad-loaderbacks" src="'.$base_url.'/sites/all/modules/ghp_photos/images/loading.gif" alt="Loader" style="display: none;">
			  	<br><br> ' 
  );
  }
  
	$form['photo_alter_text'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Alter Text'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_alter_text'])) ? $photoDataArray['photo_alter_text'] : ''),
    );
	
    $form['uploaded_by'] = array(
        '#type' => 'textfield',
        '#title' => t('Uploaded By'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['uploaded_by'])) ? $photoDataArray['uploaded_by'] : ''),
    );

    $form['photo_date'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo date'),
        '#size' => 10,
        '#maxlength' => 10,
     //   '#required' => TRUE,
        '#readonly' => TRUE,
        '#default_value' => ((isset($photoDataArray['photo_date'])) ? $photoDataArray['photo_date'] : ''),
    );
    
    $form['photo_display_date'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Display date'),
        '#size' => 10,
        '#maxlength' => 50,
     //   '#required' => TRUE,
        '#default_value' => ((isset($photoDataArray['photo_display_date'])) ? $photoDataArray['photo_display_date'] : ''),
    );
    
    $form['photo_place'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Place'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_place'])) ? stripslashes($photoDataArray['photo_place']) : ''),
    );
    
     $form['photo_source'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Source'),
        '#size' => 100,
        '#maxlength' => 255,
        //'#required' => TRUE,
        '#default_value' => ((isset($photoDataArray['photo_source'])) ? stripslashes($photoDataArray['photo_source']) : ''),
    );
    

    $form['photo_latitude'] = array(
        '#type' => 'textfield',
        '#title' => t('Latitude'),
        '#size' => 50,
        '#maxlength' => 50,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_latitude'])) ? $photoDataArray['photo_latitude'] : ''),
    );

    $form['photo_longitude'] = array(
        '#type' => 'textfield',
        '#title' => t('Longitude'),
        '#size' => 50,
        '#maxlength' => 50,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_longitude'])) ? $photoDataArray['photo_longitude'] : ''),
    );

    $form['photo_people'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo People'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_people'])) ? $photoDataArray['photo_people'] : ''),
    );

    $form['photo_significance'] = array(
        '#type' => 'textfield',
        '#title' => t('Photo Significance'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_significance'])) ? $photoDataArray['photo_significance'] : ''),
    );

    $form['photographer_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Photographer Name'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photographer_name'])) ? $photoDataArray['photographer_name'] : ''),
    );

    $form['photo_meta_keywords'] = array(
        '#type' => 'textfield',
        '#title' => t('Meta Keywords'),
        '#size' => 100,
        '#maxlength' => 255,
        '#required' => FALSE,
        '#default_value' => ((isset($photoDataArray['photo_meta_keywords'])) ? $photoDataArray['photo_meta_keywords'] : ''),
    );

    $form['photo_meta_description'] = array(
        '#type' => 'textarea',
        '#title' => t('Meta Description'),
        '#cols' => 10,
        '#rows' => 5,
        '#maxlength' => 1000,
        '#required' => FALSE,
        "#resizable" => FALSE,													  
        '#default_value' => ((isset($photoDataArray['photo_meta_description'])) ? stripslashes($photoDataArray['photo_meta_description']) : ''),
    );

    $form['photo_tag'] = array(
        '#type' => 'textfield',
        '#title' => t('Tag'),
        '#description' => t('Enter tags with comma separated (Example:- abc,def,ghi).'),
        '#size' => 100,
        '#maxlength' => 500,
        '#required' => FALSE,
        '#default_value' => ((!empty($photoTagData)) ? $photoTagData : ''),
    );

    // load ghp_references_sectionformat_validation() and ghp_references_get_sectionformat_sample_text() functions 
    // from ghp_references.module file
    module_load_include('module', 'ghp_references', 'ghp_references');

    $form['video_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Video ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Video').')'),
        '#maxlength' => 1024,
        '#default_value' => isset($video_id) ? $video_id : '',
    );
    $form['cartoon_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Cartoon ID'),
		'#maxlength' => 1024,
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Cartoon').')'),
        '#default_value' => isset($cartoon_id) ? $cartoon_id : '',
    );
    $form['audio_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Audio ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Audio').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($audio_id) ? $audio_id : '',
    );
    $form['poster_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Poster ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Poster').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($poster_id) ? $poster_id : '',
    );
    $form['stamp_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Stamp ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Stamp').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($stamp_id) ? $stamp_id : '',
    );
    $form['veroriam_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Variorum ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Variorum').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($variorum_id) ? $variorum_id : '',
    );
    $form['ghs_id'] = array(
        '#type' => 'hidden',
        '#title' => t('Reference of GHS ID'),
        '#description' => t('(Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'),
		'#maxlength' => 1024,
        '#default_value' => isset($ghs_id) ? $ghs_id : '',
    );
    $form['dtodchronology_id'] = array(
        '#type' => 'hidden',
        '#title' => t('Reference of DaytoDay Chronology ID'),
        '#description' => t('(Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'),
		'#maxlength' => 1024,
        '#default_value' => isset($dtodchronology_id) ? $dtodchronology_id : '',
    );
    $form['eventchronology_id'] = array(
        '#type' => 'hidden',
        '#title' => t('Reference of Event Chronology ID'),
        '#description' => t('(Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'),
		'#maxlength' => 1024,
        '#default_value' => isset($eventchronology_id) ? $eventchronology_id : '',
    );
    $form['collectedwork_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Volume ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Volume').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($collectedwork_id) ? $collectedwork_id : '',
    );
    $form['keytext_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Book ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Book').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($keytext_id) ? $keytext_id : '',
    );
    $form['journalbygandhiji_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Journal By Gandhiji ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('JournalByGandhiji').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($journalbygandhiji_id) ? $journalbygandhiji_id : '',
    );
    $form['others_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Journal By Others ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('JournalByOthers').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($others_id) ? $others_id : '',
    );
    $form['tributes_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Tributes ID'),
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('Tribute').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($tributes_id) ? $tributes_id : '',
    );
    $form['fundamentals_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Booksection ID'), //Reference of Fundamental Work ID
        '#description' => t('(Example:- '.ghp_references_get_sectionformat_sample_text('FundamentalWork').')'),
		'#maxlength' => 1024,
        '#default_value' => isset($fundamentals_id) ? $fundamentals_id : '',
    );
    /*$form['otherbooks_id'] = array(
        '#type' => 'textfield',
        '#title' => t('Reference of Other Book ID'),
        '#description' => t('(Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'),
		'#maxlength' => 1024,
        '#default_value' => isset($otherbooks_id) ? $otherbooks_id : '',
    );*/
	
    if (user_access('Publish Photos')) {
        $form['photo_publish_status'] = array('#type' => 'select',
            '#title' => t('Publish ?'),
            '#options' => array('Y' => t('Yes'), 'N' => t('No')),
            '#default_value' => ((isset($photoDataArray['photo_publish_status'])) ? $photoDataArray['photo_publish_status'] : 'N'),
        );
    }

    if (user_access('Approve Photos')) {
        $form['photo_approve_status'] = array('#type' => 'select',
            '#title' => t('Approve ?'),
            '#options' => array('Y' => t('Yes'), 'N' => t('No')),
            '#default_value' => ((isset($photoDataArray['photo_approve_status'])) ? $photoDataArray['photo_approve_status'] : 'N'),
        );
    }

    $form['submit'] = array('#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}

function ghp_photo_add_form_validate($form_id, $form_values) {

    $allowedExtensions = array('image/png', 'image/gif', 'image/jpeg', 'image/jpg');

    $language = $form_values['values']['language'];
    $category_id = $form_values['values']['category_id'];
    $type_of_photograph = $form_values['values']['type_of_photograph'];
    $events = $form_values['values']['events'];
    $h_site_id = $form_values['values']['h_site_id'];
	$photo_order_number = $form_values['values']['photo_order_number'];
    
    $photo_title = $form_values['values']['photo_title'];
	$photo_alter_text = $form_values['values']['photo_alter_text'];
    $photo_description = $form_values['values']['photo_description'];
    $uploaded_by = $form_values['values']['uploaded_by'];
    $photo_date = $form_values['values']['photo_date'];
    $photo_display_date = $form_values['values']['photo_display_date'];
   
    $photo_place = $form_values['values']['photo_place'];
    $photo_source = $form_values['values']['photo_source'];
    $photo_latitude = $form_values['values']['photo_latitude'];
    $photo_longitude = $form_values['values']['photo_longitude'];
    $photo_people = $form_values['values']['photo_people'];
    $photo_significance = $form_values['values']['photo_significance'];
    $photographer_name = $form_values['values']['photographer_name'];
    $photo_meta_keywords = $form_values['values']['photo_meta_keywords'];
    $photo_meta_description = $form_values['values']['photo_meta_description'];
    $photo_tag = $form_values['values']['photo_tag'];
    $photo_publish_status = $form_values['values']['photo_publish_status'];
    $photo_approve_status = $form_values['values']['photo_approve_status'];

    /* Fetch regular expression value from general setting added by Maya */
    $allowed_characters = variable_get_value('ghp_reg_exp');    
       
    /* Fetch not allowed regular expression to match for hindi and gujarati language added by Maya */
    $not_allowed_characters = get_not_allowed_characters();
    $not_allowed_characters = str_replace("\\", "\\\\", $not_allowed_characters);
    $not_allowed_characters = str_replace("/", "\/", $not_allowed_characters);
    $pattern = preg_quote($not_allowed_characters, '#');
    
    if (!empty($language)) {
        if (hasHtml($language)) {
            form_set_error('language', t('Please select valid language'));
        }
    }    

    if (!empty($category_id)) {
        if (hasHtml($category_id)) {
            form_set_error('category_id', t('Please select valid category'));
        }
    }

    if (!empty($type_of_photograph)) {
        if (hasHtml($type_of_photograph)) {
            form_set_error('type_of_photograph', t('Please select valid type of photograph'));
        }
    }
    
    if (!empty($events)) {
        if (hasHtml($events)) {
            form_set_error('events', t('Please enter valid events'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9\, ])+$/", $events)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $events)) {
            if (preg_match("#[{$pattern}]#", $events)) {
                form_set_error('events', t('Please enter valid events. Allowed special characters are '.$allowed_characters));
            }
        }
    }
    
    if (!empty($h_site_id)) {
        if (hasHtml($h_site_id)) {
            form_set_error('h_site_id', t('Please select valid heritage site name'));
        }
    }
	
	    
    if (!empty($photo_order_number)) {
        if (hasHtml($photo_order_number)) {
            form_set_error('photo_order_number', t('Please Enter valid Order Number.'));
        }
    }
    
    if (!empty($photo_title)) {
        if (hasHtml($photo_title)) {
            form_set_error('photo_title', t('Please enter valid photo title'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_title)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $photo_title)) {
            if (preg_match("#[{$pattern}]#", $photo_title)) {
                form_set_error('photo_title', t('Please enter valid photo title. Allowed special characters are '.$allowed_characters));
            }
        }
    }

	if (!empty($photo_alter_text)) {
        if (hasHtml($photo_alter_text)) {
            form_set_error('photo_alter_text', t('Please enter valid photo alter text'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_alter_text)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $photo_alter_text)) {
            if (preg_match("#[{$pattern}]#", $photo_alter_text)) {
                form_set_error('photo_alter_text', t('Please enter valid photo alter text. Allowed special characters are '.$allowed_characters));
            }
        }
    }
	
    if (!empty($photo_description)) {
        if (hasHtml($photo_description)) {
            form_set_error('photo_description', t('Please enter valid photo description'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_description)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $photo_description)) {
            if (preg_match("#[{$pattern}]#", $photo_description)) {
                form_set_error('photo_description', t('Please enter valid photo description. Allowed special characters are '.$allowed_characters));
            }
        }
    } 
    
    if (!empty($uploaded_by)) {
        if (hasHtml($uploaded_by)) {
            form_set_error('uploaded_by', t('Please enter valid uploaded by'));
        } else {            
            if (!preg_match("/^([0-9a-zA-Z\- ])+$/", $uploaded_by)) {            
                form_set_error('uploaded_by', t('Please enter valid uploaded by'));
            }
        }
    }  
        
    
    if (!empty($photo_date)) {
        if (!preg_match("/^[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])$/",$photo_date))
	    {
	    	form_set_error('photo_date', t('Please enter valid photo date. (Ex.: YYYY-MM-DD)'));
	    }	
			
    }
    
    /*if (!empty($photo_display_date)) {
    	if (!preg_match("/^[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])$/",$photo_display_date))
	    {
	    	form_set_error('photo_display_date', t('Please enter valid photo display date. (Ex.: YYYY-MM-DD)'));
	    }
        
    }*/ 
    
    
    if (!empty($photo_place)) {
        if (hasHtml($photo_place)) {
            form_set_error('photo_place', t('Please enter valid photo place'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_place)) {            
            if (preg_match("#[{$pattern}]#", $photo_place)) {
                form_set_error('photo_place', t('Please enter valid photo place. Allowed special characters are '.$allowed_characters));
            }
        }
    }
    
    if (!empty($photo_latitude)) {
        if (hasHtml($photo_latitude)) {
            form_set_error('photo_latitude', t('Please enter valid latitude'));
        } else {
            if (!preg_match("/^([0-9\.\-\+ ])+$/", $photo_latitude)) {            
                form_set_error('photo_latitude', t('Please enter valid latitude.'));
            }
        }
    }    

    if (!empty($photo_longitude)) {
        if (hasHtml($photo_longitude)) {
            form_set_error('photo_longitude', t('Please enter valid longitude'));
        } else {
            if (!preg_match("/^([0-9\.\-\+ ])+$/", $photo_longitude)) {            
                form_set_error('photo_longitude', t('Please enter valid longitude.'));
            }
        }
    } 

    if (!empty($photo_people)) {
        if (hasHtml($photo_people)) {
            form_set_error('photo_people', t('Please enter valid photo people'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_people)) {            
            if (preg_match("#[{$pattern}]#", $photo_people)) {
                form_set_error('photo_people', t('Please enter valid photo people. Allowed special characters are '.$allowed_characters));
            }
        }
    }
    
     if (!empty($photo_source)) {
        if (hasHtml($photo_source)) {
            form_set_error('photo_source', t('Please enter valid photo source'));
        } else {                        
            if (preg_match("#[{$pattern}]#", $photo_source)) {
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_source)) {
                form_set_error('photo_source', t('Please enter valid photo source. Allowed special characters are '.$allowed_characters));
            }
        }
    }
    

    if (!empty($photo_significance)) {
        if (hasHtml($photo_significance)) {
            form_set_error('photo_significance', t('Please enter valid photo significance'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_significance)) {                            
            if (preg_match("#[{$pattern}]#", $photo_significance)) {
                form_set_error('photo_significance', t('Please enter valid photo significance. Allowed special characters are '.$allowed_characters));
            }
        }
    } 
    
    if (!empty($photographer_name)) {
        if (hasHtml($photographer_name)) {
            form_set_error('photographer_name', t('Please enter valid photographer name'));
        } else {            
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photographer_name)) {            
            if (preg_match("#[{$pattern}]#", $photographer_name)) {
                form_set_error('photographer_name', t('Please enter valid photographer name. Allowed special characters are '.$allowed_characters));
            }
        }
    } 
    
    if (!empty($photo_meta_keywords)) {
        if (hasHtml($photo_meta_keywords)) {
            form_set_error('photo_meta_keywords', t('Please enter valid meta keywords'));
        } else {
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_meta_keywords)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $photo_meta_keywords)) {
            if (preg_match("#[{$pattern}]#", $photo_meta_keywords)) {    
                form_set_error('photo_meta_keywords', t('Please enter valid meta keywords. Allowed special characters are '.$allowed_characters));
            }
        }
    }

    if (!empty($photo_meta_description)) {
        if (hasHtml($photo_meta_description)){
            form_set_error('photo_meta_description', t('Please enter valid meta description'));
        } else {
            //if (!preg_match("/^([a-zA-Z0-9.\:\)\(\;\,\-\'\" ])+$/", $photo_meta_description)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $photo_meta_description)) {
            if (preg_match("#[{$pattern}]#", $photo_meta_description)) {    
                form_set_error('photo_meta_description', t('Please enter valid meta description. Allowed special characters are '.$allowed_characters));
            }
        }
    }

    if (!empty($photo_tag)) {
        if (hasHtml($photo_tag)){
              form_set_error('photo_tag', t('Please enter valid tags'));
        } else {             
             //if (!preg_match("/^([0-9a-zA-Z\, ])+$/", $photo_tag)) {
            if (preg_match("#[{$pattern}]#", $photo_tag)) {
                form_set_error('photo_tag', t('Please enter valid tags. Allowed special characters are '.$allowed_characters));
            }
        }
    }  
  	
	//validation of the reference id

    // load ghp_references_sectionformat_validation() and ghp_references_get_sectionformat_sample_text() functions 
    // from ghp_references.module file
    module_load_include('module', 'ghp_references', 'ghp_references');
	
    if (!empty($form_values['values']['video_id'])) {
    	// if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['video_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['video_id'], 'Video') != 'valid' )
        {
        	form_set_error('video_id', t('Please enter valid reference of video id. (Example:- '.ghp_references_get_sectionformat_sample_text('Video').')'));
   		}    
   	}
    
    if (!empty($form_values['values']['cartoon_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['cartoon_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['cartoon_id'], 'Cartoon') != 'valid' )
        {
            form_set_error('cartoon_id', t('Please enter valid reference of cartoon id. (Example:- '.ghp_references_get_sectionformat_sample_text('Cartoon').')'));
    	}    
    }
	
	if (!empty($form_values['values']['audio_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['audio_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['audio_id'], 'Audio') != 'valid' )
        {
            form_set_error('audio_id', t('Please enter valid reference of audio id. (Example:- '.ghp_references_get_sectionformat_sample_text('Audio').')'));
    	}    
    }
   	
   	if (!empty($form_values['values']['poster_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['poster_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['poster_id'], 'Poster') != 'valid' )
        {
            form_set_error('poster_id', t('Please enter valid reference of poster id. (Example:- '.ghp_references_get_sectionformat_sample_text('Poster').')'));
    	}    
    }
    
    if (!empty($form_values['values']['stamp_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['stamp_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['stamp_id'], 'Stamp') != 'valid' )
        {
            form_set_error('stamp_id', t('Please enter valid reference of stamp id. (Example:- '.ghp_references_get_sectionformat_sample_text('Stamp').')'));
    	}    
    }
    if (!empty($form_values['values']['veroriam_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['veroriam_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['veroriam_id'], 'Variorum') != 'valid' )
        {
            form_set_error('veroriam_id', t('Please enter valid reference of variorum id. (Example:- '.ghp_references_get_sectionformat_sample_text('Variorum').')'));
    	}    
    }
    if (!empty($form_values['values']['ghs_id'])) {
        if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['ghs_id']))
        {
            form_set_error('ghs_id', t('Please enter valid reference of ghs id. (Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'));
    	}    
    }
    if (!empty($form_values['values']['dtodchronology_id'])) {
        if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['dtodchronology_id']))
        {
            form_set_error('dtodchronology_id', t('Please enter valid reference of daytoday chronology id. (Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'));
    	}    
    }
    if (!empty($form_values['values']['eventchronology_id'])) {
        if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['eventchronology_id']))
        {
            form_set_error('eventchronology_id', t('Please enter valid reference of eventchronology id. (Example:- 1:1,2,3,4,5;11:11,12,13,14,15;)'));
    	}    
    }
    if (!empty($form_values['values']['collectedwork_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['collectedwork_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['collectedwork_id'], 'Volume') != 'valid' )
        {
            form_set_error('collectedwork_id', t('Please enter valid reference of volume id. (Example:- '.ghp_references_get_sectionformat_sample_text('Volume').')'));
    	}    
    }
    if (!empty($form_values['values']['keytext_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['keytext_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['keytext_id'], 'Book') != 'valid' )
        {
            form_set_error('keytext_id', t('Please enter valid reference of book id. (Example:- '.ghp_references_get_sectionformat_sample_text('Book').')'));
    	}    
    }
    if (!empty($form_values['values']['journalbygandhiji_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['journalbygandhiji_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['journalbygandhiji_id'], 'JournalByGandhiji') != 'valid' )
        {
            form_set_error('journalbygandhiji_id', t('Please enter valid reference of journal by gandhiji id. (Example:- '.ghp_references_get_sectionformat_sample_text('JournalByGandhiji').')'));
    	}    
    }
    if (!empty($form_values['values']['others_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['others_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['others_id'], 'JournalByOthers') != 'valid' )
        {
            form_set_error('others_id', t('Please enter valid reference of journal by others id. (Example:- '.ghp_references_get_sectionformat_sample_text('JournalByOthers').')'));
    	}    
    }
    if (!empty($form_values['values']['tributes_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['tributes_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['tributes_id'], 'Tribute') != 'valid' )
        {
            form_set_error('tributes_id', t('Please enter valid reference of tributes id. (Example:- '.ghp_references_get_sectionformat_sample_text('Tribute').')'));
    	}    
    }
    if (!empty($form_values['values']['fundamentals_id'])) {
        // if (!preg_match('/^[0-9,:;]*$/',$form_values['values']['fundamentals_id']))
        if( ghp_references_sectionformat_validation($form_values['values']['fundamentals_id'], 'FundamentalWork') != 'valid' )
        {
            form_set_error('fundamentals_id', t('Please enter valid reference of booksection id. (Example:- '.ghp_references_get_sectionformat_sample_text('FundamentalWork').')'));
    	}    
    }
    // end of the reference validation value
     
    if (!empty($photo_publish_status)) {
        if (hasHtml($photo_publish_status)) {
            form_set_error('photo_publish_status', t('Please select valid photo publish status'));
        }
    }

    if (!empty($photo_approve_status)) {
        if (hasHtml($photo_approve_status)) {
            form_set_error('photo_approve_status', t('Please select valid photo approve status'));
        }
    }    

    if (!empty($form_values['values']['photoId'])) {
        if (!empty($_FILES['files']['name']['photo_filename'])) {
            if (!in_array($_FILES['files']['type']['photo_filename'], $allowedExtensions)) {
                form_set_error('photo_filename', t('Please select valid file.'));
            }
            
            if (!in_array(mime_content_type($_FILES['files']['tmp_name']['photo_filename']), $allowedExtensions)) {
                form_set_error('photo_filename', t('Please select valid file.'));
            }            
        }
    } else {
        if (!empty($_FILES['files']['name']['photo_filename'])) 
		{
            if (!in_array($_FILES['files']['type']['photo_filename'], $allowedExtensions)) {
                form_set_error('photo_filename', t('Please select valid file.'));
            }

            if (!in_array(mime_content_type($_FILES['files']['tmp_name']['photo_filename']), $allowedExtensions)) {
                form_set_error('photo_filename', t('Please select valid file.'));
            }
        }
    }
}

function ghp_photo_add_form_submit($form_id, $form_values) {

    global $user;
    $ghp_data_root_path = variable_get('ghp_data_root_path');
   // echo $ghp_data_root_path; exit; 
    if (!empty($form_values['values']['photoId']) && !empty($form_values['values']['photo_filename_hidden']) && empty($_FILES['files']['name']['photo_filename'])) {
        $filename = $form_values['values']['photo_filename_hidden'];
    } else 
	  {
		if(!empty($_FILES['files']['name']['photo_filename']))
		{	

					$target_path = $ghp_data_root_path . 'ghp_photos/';
					$target_small_path = $target_path . 'small/';
					$target_medium_path = $target_path . 'medium/';
					$target_large_path = $target_path . 'print/8x11/';

					// UNLINK OLD IMAGE
					if (!empty($form_values['values']['photo_filename_hidden']) && (is_file($target_path . $form_values['values']['photo_filename_hidden']))) {
						if (is_file($target_path . $form_values['values']['photo_filename_hidden'])) {
							unlink($target_path . $form_values['values']['photo_filename_hidden']);
						}

						if (is_file($target_small_path . $form_values['values']['photo_filename_hidden'])) {
							unlink($target_small_path . $form_values['values']['photo_filename_hidden']);
						}

						if (is_file($target_medium_path . $form_values['values']['photo_filename_hidden'])) {
							unlink($target_medium_path . $form_values['values']['photo_filename_hidden']);
						}
						
						if (is_file($target_large_path . $form_values['values']['photo_filename_hidden'])) {
							unlink($target_large_path . $form_values['values']['photo_filename_hidden']);
						}
					}

					$oldFilename = basename($_FILES['files']['name']['photo_filename']);
					$fileExtension = end(explode('.', $oldFilename));
					$filename = microtime(true) . '.' . $fileExtension;

					$photo_target_path = $target_path . $filename;
					$photo_target_small_path = $target_small_path . $filename;
					$photo_target_medium_path = $target_medium_path . $filename;
					$photo_target_large_path = $target_large_path . $filename;

					if (move_uploaded_file($_FILES['files']['tmp_name']['photo_filename'], $photo_target_path)) {
						
					} else {
						
					}
					
					// Resize Image SMALL
					$imgSmall = image_load($photo_target_path);
					image_resize($imgSmall, 42, 42);
					image_save($imgSmall, $photo_target_small_path);

					// Resize Image MEDIUM        
					$imgMedium = image_load($photo_target_path);
					image_resize($imgMedium, 129, 135);
					image_save($imgMedium, $photo_target_medium_path);
					
					// large image
					$imglarge = image_load($photo_target_path);
					image_save($imglarge, $photo_target_large_path);
		}
    }

    // INSERT IN PHOTO MASTER
    if (empty($form_values['values']['photoId'])) {
        
        
        // START ENTRY IN PHOTOS MASTER TABLE
        $photoEntry = array('photo_filename' => $filename,
            'category_id' => mysql_escape_string($form_values['values']['category_id']),
            'h_site_id' => mysql_escape_string($form_values['values']['h_site_id']),
			'copyright_id' => mysql_escape_string($form_values['values']['copyright_id']),
            'photo_display_date' => mysql_escape_string($form_values['values']['photo_display_date']),
			'photo_order_number' => mysql_escape_string($form_values['values']['photo_order_number']),
            'uploaded_by' => mysql_escape_string($form_values['values']['uploaded_by']),
            'uploaded_date' => date('Y-m-d'),
            'modified_date' => date('Y-m-d H:i:s'),
            'photo_created_by' => 1,
            'photo_latitude' => mysql_escape_string($form_values['values']['photo_latitude']),
            'photo_longitude' => mysql_escape_string($form_values['values']['photo_longitude']),
            'created_by' => $user->uid
        );
        
        if((!empty($form_values['values']['photo_date'])) && $form_values['values']['photo_date'] != "0000-00-00")
        {             
            $photoEntry['photo_date'] =  mysql_escape_string($form_values['values']['photo_date']);
             
        }
        else
        {
			$photoEntry['photo_date'] = NULL;

		}
		
        ///hasyarekha Code for permision issue
        if ((user_access('Publish Photos'))) {
            $photoEntry = array_merge($photoEntry, array('photo_publish_status' => $form_values['values']['photo_publish_status'], 'published_by' => $user->uid));
        } else {
            $photoEntry = array_merge($photoEntry, array('photo_publish_status' => 'N'));
        }

        if ((user_access('Approve Photos'))) {
            $photoEntry = array_merge($photoEntry, array('photo_approve_status' => $form_values['values']['photo_approve_status'], 'approved_by' => $user->uid));
        } else {
            $photoEntry = array_merge($photoEntry, array('photo_approve_status' => 'N'));
        }

        //hasyarekha Code over for permission issue

        try {
            $photo_id = db_insert('photo_master')
                    ->fields($photoEntry)
                    ->execute();
        } catch (Exception $e) {
            $errorFlag = true;
            drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
        // STOP ENTRY IN PHOTOS MASTER TABLE
    } else {
                    
        $pMUpdateEntry = array('photo_filename' => $filename,
            'category_id' => mysql_escape_string($form_values['values']['category_id']),
            'h_site_id' => mysql_escape_string($form_values['values']['h_site_id']),
			'copyright_id' => mysql_escape_string($form_values['values']['copyright_id']),
            'photo_display_date' => mysql_escape_string($form_values['values']['photo_display_date']),
			'photo_order_number' => mysql_escape_string($form_values['values']['photo_order_number']),
            'uploaded_by' => mysql_escape_string($form_values['values']['uploaded_by']),
            'modified_date' => date('Y-m-d H:i:s'),
            'photo_created_by' => 1,
            'photo_latitude' => mysql_escape_string($form_values['values']['photo_latitude']),
            'photo_longitude' => mysql_escape_string($form_values['values']['photo_longitude']),
            'modified_by' => $user->uid
        );

        if((!empty($form_values['values']['photo_date'])) && $form_values['values']['photo_date'] != "0000-00-00")
        {             
             $pMUpdateEntry['photo_date'] =  mysql_escape_string($form_values['values']['photo_date']);
        }
        else
        {
			$pMUpdateEntry['photo_date'] = NULL;
		}
        
        if ((user_access('Publish Photos'))) {
            $pMUpdateEntry = array_merge($pMUpdateEntry, array('photo_publish_status' => $form_values['values']['photo_publish_status'], 'published_by' => $user->uid));
        }

        if ((user_access('Approve Photos'))) {
            $pMUpdateEntry = array_merge($pMUpdateEntry, array('photo_approve_status' => $form_values['values']['photo_approve_status'], 'approved_by' => $user->uid));
        }

        try {
            // START UPDATE ENTRY IN PHOTO MASTER TABLE        
            $updateResult = db_update('photo_master')
                    ->fields($pMUpdateEntry)
                    ->condition('photo_id', $form_values['values']['photoId'])
                    ->execute();
            // STOP UPDATE ENTRY IN PHOTO MASTER TABLE  

            $photo_id = $form_values['values']['photoId'];
        } catch (Exception $e) {
            drupal_set_message(t('db_update failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
    }

    if (empty($form_values['values']['photoId'])) {
        // START ENTRY IN PHOTO DETAIL TABLE
        $photoDetailEntry = array('photo_id' => $photo_id,
            'language_code' => mysql_escape_string($form_values['values']['language']),
            'photo_title' => mysql_escape_string($form_values['values']['photo_title']),
			'photo_alter_text' => mysql_escape_string($form_values['values']['photo_alter_text']),
            'photo_description' => mysql_escape_string($form_values['values']['photo_description']),
            'photo_place' => mysql_escape_string($form_values['values']['photo_place']),
            'photo_people' => mysql_escape_string($form_values['values']['photo_people']),
            'photo_source' => mysql_escape_string($form_values['values']['photo_source']),
            'photo_significance' => mysql_escape_string($form_values['values']['photo_significance']),
            'photographer_name' => mysql_escape_string($form_values['values']['photographer_name']),
            'photo_meta_keywords' => mysql_escape_string($form_values['values']['photo_meta_keywords']),
            'photo_meta_description' => mysql_escape_string($form_values['values']['photo_meta_description']),
            'events' => mysql_escape_string($form_values['values']['events']),
            'type_of_photograph' => mysql_escape_string($form_values['values']['type_of_photograph'])
        );

        try {
            $photo_detail_id = db_insert('photo_detail')
                    ->fields($photoDetailEntry)
                    ->execute();
        } catch (Exception $e) {
            $errorFlag = true;
            drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
        // STOP ENTRY IN PHOTO DETAIL TABLE
    } else {

        // CHECK FOR DATA EXIST OR NOT
        $detailExitstCount = get_photo_detail_exist($form_values['values']['language'], $photo_id);

        if ($detailExitstCount == 0) {
            // START ENTRY IN PHOTO DETAIL TABLE
            $photoDetailEntry = array('photo_id' => $photo_id,
                'language_code' => mysql_escape_string($form_values['values']['language']),
                'photo_title' => mysql_escape_string($form_values['values']['photo_title']),
				'photo_alter_text' => mysql_escape_string($form_values['values']['photo_alter_text']),
                'photo_description' => mysql_escape_string($form_values['values']['photo_description']),
                'photo_place' => mysql_escape_string($form_values['values']['photo_place']),
                'photo_source' => mysql_escape_string($form_values['values']['photo_source']),
                'photo_people' => mysql_escape_string($form_values['values']['photo_people']),
                'photo_significance' => mysql_escape_string($form_values['values']['photo_significance']),
                'photographer_name' => mysql_escape_string($form_values['values']['photographer_name']),
                'photo_meta_keywords' => mysql_escape_string($form_values['values']['photo_meta_keywords']),
                'photo_meta_description' => mysql_escape_string($form_values['values']['photo_meta_description']),
                'events' => mysql_escape_string($form_values['values']['events']),
                'type_of_photograph' => mysql_escape_string($form_values['values']['type_of_photograph'])
            );

            try {
                $photo_detail_id = db_insert('photo_detail')
                        ->fields($photoDetailEntry)
                        ->execute();
            } catch (Exception $e) {
                $errorFlag = true;
                drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
            }
            // STOP ENTRY IN PHOTO DETAIL TABLE
        } else {
            // START UPDATE IN PHOTO DETAIL TABLE
            $photoDetailUEntry = array('photo_id' => $photo_id,
                'language_code' => mysql_escape_string($form_values['values']['language']),
                'photo_title' => mysql_escape_string($form_values['values']['photo_title']),
				'photo_alter_text' => mysql_escape_string($form_values['values']['photo_alter_text']),
                'photo_description' => mysql_escape_string($form_values['values']['photo_description']),
                'photo_place' => mysql_escape_string($form_values['values']['photo_place']),
                'photo_people' => mysql_escape_string($form_values['values']['photo_people']),
                'photo_source' => mysql_escape_string($form_values['values']['photo_source']),
                'photo_significance' => mysql_escape_string($form_values['values']['photo_significance']),
                'photographer_name' => mysql_escape_string($form_values['values']['photographer_name']),
                'photo_meta_keywords' => mysql_escape_string($form_values['values']['photo_meta_keywords']),
                'photo_meta_description' => mysql_escape_string($form_values['values']['photo_meta_description']),
                'events' => mysql_escape_string($form_values['values']['events']),
                'type_of_photograph' => mysql_escape_string($form_values['values']['type_of_photograph'])
            );

            try {
                $photoUpdateResult = db_update('photo_detail')
                        ->fields($photoDetailUEntry)
                        ->condition('photo_id', $photoDetailUEntry['photo_id'])
                        ->condition('language_code', $photoDetailUEntry['language_code'])
                        ->execute();
            } catch (Exception $e) {
                $errorFlag = true;
                drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
            }
            // STOP UPDATE IN PHOTO DETAIL TABLE 
        }
    }

    if (!empty($form_values['values']['photo_tag'])) {

        $photoTagArray = explode(',', $form_values['values']['photo_tag']);

        if (count($photoTagArray) > 0) {

            if (!empty($form_values['values']['photoId'])) {
                $result = db_delete('photo_tags')
                        ->condition('photo_id', $form_values['values']['photoId'])
                        ->execute();
            }
            foreach ($photoTagArray as $photoTag) {
                $photoTagEntry = array('photo_id' => $photo_id,
                    'created_date' => date('Y-m-d'),
                    'modified_date' => date('Y-m-d'),
                    'tag_name' => mysql_escape_string($photoTag),
                );

                // START ENTRY IN PHOTO TAG TABLE
                try {
                    $photo_tag_id = db_insert('photo_tags')
                            ->fields($photoTagEntry)
                            ->execute();
                } catch (Exception $e) {
                    $errorFlag = true;
                    drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
                }
                // STOP ENTRY IN PHOTO TAG TABLE                
            }
        }
    }

	//References of other ID

	 // Delete all reference item during edit.
    if ($form_values['values']['photoId'] != "") {
        $deleteRef = db_delete('photo_references')->condition('ref_id', $photo_id)->execute();
    }
    // After entering photo entry need to add all reference items.
	
    // Reference Video ID
    if ($form_values['values']['video_id'] != "") {
        $video = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Video',
            'section_format' => $form_values['values']['video_id']
        );
        $videoQuery = db_insert('photo_references')->fields($video)->execute();
    }

    // Reference Cartoon ID
    if ($form_values['values']['cartoon_id'] != "") {
        $cartoon = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Cartoon',
            'section_format' => $form_values['values']['cartoon_id']
        );
        $cartoonQuery = db_insert('photo_references')->fields($cartoon)->execute();
    }

    // Reference Audio ID
    if ($form_values['values']['audio_id'] != "") {
        $audio = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Audio',
            'section_format' => $form_values['values']['audio_id']
        );
        $audioQuery = db_insert('photo_references')->fields($audio)->execute();
    }

    // Reference Poster ID
    if ($form_values['values']['poster_id'] != "") {
        $poster = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Poster',
            'section_format' => $form_values['values']['poster_id']
        );
        $posterQuery = db_insert('photo_references')->fields($poster)->execute();
    }
    
    // Reference Other Stamp ID
    if ($form_values['values']['stamp_id'] != "") {
        $stamp = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Stamp',
            'section_format' => $form_values['values']['stamp_id']
        );
        $stampQuery = db_insert('photo_references')->fields($stamp)->execute();
    }
 
    // Reference Other Variorum ID
    if ($form_values['values']['veroriam_id'] != "") {
        $veroriam = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Variorum',
            'section_format' => $form_values['values']['veroriam_id']
        );
        $stampQuery = db_insert('photo_references')->fields($veroriam)->execute();
    }

    // Reference GHS ID
    if ($form_values['values']['ghs_id'] != "") {
        $ghs = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'GHS',
            'section_format' => $form_values['values']['ghs_id']
        );
        $ghsQuery = db_insert('photo_references')->fields($ghs)->execute();
    }
    // Reference DayToDayChronology ID
    if ($form_values['values']['dtodchronology_id'] != "") {
        $dtodchronology = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'DayToDayChronology',
            'section_format' => $form_values['values']['dtodchronology_id']
        );
        $dtodchronologyQuery = db_insert('photo_references')->fields($dtodchronology)->execute();
    }
    // Reference EventChronology ID
    if ($form_values['values']['eventchronology_id'] != "") {
        $eventchronology = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'EventChronology',
            'section_format' => $form_values['values']['eventchronology_id']
        );
        $eventchronologyQuery = db_insert('photo_references')->fields($eventchronology)->execute();
    }
    // Reference CollectedWork ID
    if ($form_values['values']['collectedwork_id'] != "") {
        $collectedwork = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Volume',
            'section_format' => $form_values['values']['collectedwork_id']
        );
        $collectedworkQuery = db_insert('photo_references')->fields($collectedwork)->execute();
    }
    // Reference Keytext ID
    if ($form_values['values']['keytext_id'] != "") {
        $keytext = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Book',
            'section_format' => $form_values['values']['keytext_id']
        );
        $keytextQuery = db_insert('photo_references')->fields($keytext)->execute();
    }
    // Reference JournalByGandhiji ID
    if ($form_values['values']['journalbygandhiji_id'] != "") {
        $journalbygandhiji = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'JournalByGandhiji',
            'section_format' => $form_values['values']['journalbygandhiji_id']
        );
        $journalbygandhijiQuery = db_insert('photo_references')->fields($journalbygandhiji)->execute();
    }
    // Reference Others ID
    if ($form_values['values']['others_id'] != "") {
        $others = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'JournalByOthers',
            'section_format' => $form_values['values']['others_id']
        );
        $othersQuery = db_insert('photo_references')->fields($others)->execute();
    }
    // Reference Tributes ID
    if ($form_values['values']['tributes_id'] != "") {
        $tributes = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'Tribute',
            'section_format' => $form_values['values']['tributes_id']
        );
        $tributesQuery = db_insert('photo_references')->fields($tributes)->execute();
    }
    // Reference Fundamentals ID
    if ($form_values['values']['fundamentals_id'] != "") {
        $fundamentals = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'FundamentalWork',
            'section_format' => $form_values['values']['fundamentals_id']
        );
        $fundamentalsQuery = db_insert('photo_references')->fields($fundamentals)->execute();
    }
    // Reference Otherbooks ID
    if ($form_values['values']['otherbooks_id'] != "") {
        $otherbooks = array(
            'ref_id' => $photo_id,
            'module_name' => 'Photo',
            'section_name' => 'OtherBooks',
            'section_format' => $form_values['values']['otherbooks_id']
        );
        $otherbooksQuery = db_insert('photo_references')->fields($otherbooks)->execute();
    }
	
	
    if (empty($form_values['values']['photoId'])) {
        drupal_set_message(t("Photo added successfully."));
    } else {
        drupal_set_message(t("Photo updated successfully."));
    }
    //drupal_goto("admin/ghp_photos/add/" . $form_values['values']['language'] . '/' . $photo_id);    
    if(!empty($form_values['values']['language'])){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $form_values['values']['language'],
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function photos_delete() {
    $id = check_plain($_POST["del_id"]);
    $lang_code = check_plain($_POST["lang_code"]);
    $ghp_data_root_path = variable_get('ghp_data_root_path');
    
    if (isset($id) && $id != '') {

        // DELETE FROM PHOTO DETAIL TABLE
        $result = db_delete('photo_detail')
                ->condition('photo_id', $id)
                ->condition('language_code', $lang_code)
                ->execute();

		// Reference delete
        $referenceDelete = db_delete('photo_references')->condition('ref_id', $id)->execute();

        // START FOR RECORD COUNT
        $photoCountQuery = "select * from photo_detail where photo_id = " . $id . "";
        $photoCountResultQuery = db_query($photoCountQuery);
        $countPhotos = $photoCountResultQuery->rowCount();
        // END FOR RECORD COUNT        

        if ($countPhotos == 0) {
            // UNLINK IMAGE
            $photoDataArray = get_all_photos($id);
            $photo_filename = $photoDataArray['photo_filename'];

            $target_path = $ghp_data_root_path . 'ghp_photos/';
            $target_small_path = $target_path.'small/';
            $target_medium_path = $target_path.'medium/';
            $target_large_path = $target_path.'print/8x11/';

            if (is_file($target_path . $photo_filename)) {
                unlink($target_path . $photo_filename);
            }

            if (is_file($target_small_path . $photo_filename)) {
                unlink($target_small_path . $photo_filename);
            }

            if (is_file($target_medium_path . $photo_filename)) {
                unlink($target_medium_path . $photo_filename);
            }
            
            if (is_file($target_large_path . $photo_filename)) {
                unlink($target_large_path . $photo_filename);
            }

            // DELETE FROM PHOTO TAGS TABLE
            $resultTag = db_delete('photo_tags')
                    ->condition('photo_id', $id)
                    ->execute();

            // DELETE FROM PHOTO MASTER TABLE
            $resultMaster = db_delete('photo_master')
                    ->condition('photo_id', $id)
                    ->execute();
                    
            // DELETE FROM PHOTO TAGS TABLE
            $resultfacTag = db_delete('photo_tag')
                    ->condition('pic_id', $id)
                    ->execute();
        
        }

        drupal_set_message(t("Photo has been deleted successfully."));
    } else {
        drupal_set_message(t('Invalid Photo Id.'));
    }
    if(!empty($lang_code)){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $lang_code,
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function photos_deleteall() {
    
    $lang_code = check_plain($_POST["lang_code"]);
    $ghp_data_root_path = variable_get('ghp_data_root_path');
    if (!empty($_POST['photoselect'])) {        
        $photoids = $_POST['photoselect'];
        
        foreach($photoids as $row){       
            // DELETE FROM PHOTO DETAIL TABLE
            $result = db_delete('photo_detail')
                    ->condition('photo_id', $row)
                    ->condition('language_code', $lang_code)
                    ->execute();

            // START FOR RECORD COUNT
            $photoCountQuery = "select * from photo_detail where photo_id = " . $row . "";
            $photoCountResultQuery = db_query($photoCountQuery);
            $countPhotos = $photoCountResultQuery->rowCount();
            // END FOR RECORD COUNT        
            
             if ($countPhotos == 0) {
                // UNLINK IMAGE
                $photoDataArray = get_all_photos($row);
                $photo_filename = $photoDataArray['photo_filename'];

                $target_path = $ghp_data_root_path . 'ghp_photos/';
                $target_small_path = $target_path.'small/';
                $target_medium_path = $target_path.'medium/';
                $target_large_path = $target_path.'print/8x11/';

                if (is_file($target_path . $photo_filename)) {
                    unlink($target_path . $photo_filename);
                }

                if (is_file($target_small_path . $photo_filename)) {
                    unlink($target_small_path . $photo_filename);
                }

                if (is_file($target_medium_path . $photo_filename)) {
                    unlink($target_medium_path . $photo_filename);
                }
                
                if (is_file($target_large_path . $photo_filename)) {
                    unlink($target_large_path . $photo_filename);
                }

                // DELETE FROM PHOTO TAGS TABLE
                $resultTag = db_delete('photo_tags')
                        ->condition('photo_id', $row)
                        ->execute();

                // DELETE FROM PHOTO MASTER TABLE
                $resultMaster = db_delete('photo_master')
                        ->condition('photo_id', $row)
                        ->execute();
                // DELETE FROM PHOTO REFERENCES TABLE
                $resultreference = db_delete('photo_references')
                        ->condition('ref_id', $row)
                        ->execute();
                // DELETE FROM FACEBOOK LIKE PHOTO TAG TABLE
                $resultfaceTag = db_delete('photo_tag')
                        ->condition('pic_id', $row)
                        ->execute();        
            }
        }
        drupal_set_message(t("Photos has been deleted successfully."));
    }
    else
    {
        drupal_set_message(t("No record found to delete."));
    }  
    if(!empty($lang_code)){
        drupal_goto("admin/ghp_photos",array(
        'query' => array(
            'languagename' => $lang_code,
        )));
    }
    else
    {
        drupal_goto("admin/ghp_photos");
    }
}

function get_all_photos($id) {

    $sqlPhoto = "select pm.*,pd.language_code  from photo_master pm LEFT JOIN 
                photo_detail pd ON pm.photo_id = pd.photo_id
                where pm.photo_id = '" . $id . "' ";
    $results = db_query($sqlPhoto)->fetchAssoc();

    return $results;
}

function ghpPhotoAdminDownload($arg) {
    
    $photoArray = get_all_photos($arg);
    
    $filename = $photoArray['photo_filename'];
    
    $ghp_data_root_path = variable_get('ghp_data_root_path');
    $downloadfile = $ghp_data_root_path . 'ghp_photos/';

    $target_path = $downloadfile . $filename;

    if (is_file($target_path)) {
        header("Content-type: application/force-download");
        header('Content-Disposition: inline; filename="' . $target_path . '"');
        header("Content-Transfer-Encoding: Binary");
        header("Content-Length: " . filesize($downloadfile . $filename));
        header('Content-Disposition: attachment; filename="' . $filename . '"');
        readfile("$target_path");
        exit;
    } else {
        drupal_set_message(t('Photo Not Found.'));
        drupal_goto("admin/ghp_photos/add/" . $photoArray['language_code'] . "/" . $arg);
    }
}

function photo_import_add_form() {
    
    global $base_url;
    $languageArray = get_languages();
    
    $adminRenamePath = ((variable_get('rename_admin_path') != NULL) ? variable_get('rename_admin_path_value') : 'admin');

    $form = array('#attributes' => array('enctype' => "multipart/form-data", "class" => "ghpForm"));

    $form['language'] = array('#type' => 'select',
        '#title' => t('Select Language'),
        '#options' => $languageArray,
        '#required' => TRUE
    );

    $form['photo_csv'] = array(
        '#type' => 'file',
        '#title' => t('Upload Photo Content: <span title="This field is required." class="form-required">*</span>'),
        '#description' => t('Allowed file .csv only.').'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="'.$base_url.'/'.$adminRenamePath.'/download_sample_csv/photo_content_import">Download Sample CSV</a>',
        '#size' => 20,
    );

    $form['submit'] = array('#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}

function photo_import_add_form_validate($form_id, $form_values) {
    $allowedExtensions = array("text/plain", "application/octet-stream", "text/comma-separated-values", "text/csv", "application/csv", "application/force-download", "text/html", "application/x-csv", "application/vnd.ms-excel");
    
    $language = $form_values['values']['language'];

    if (!empty($language)) {
        if (hasHtml($language)) {
            form_set_error('language', t('Please select valid language'));
        }
    }    

    if (empty($_FILES['files']['name']['photo_csv'])) {
        form_set_error('book_csv', t('Upload Photo Content field is required.'));
    } else {
        if (!in_array($_FILES['files']['type']['photo_csv'], $allowedExtensions)) {
            form_set_error('photo_csv', t('Please select valid file.'));
        }

        if (!in_array(mime_content_type($_FILES['files']['tmp_name']['photo_csv']), $allowedExtensions)) {
            form_set_error('photo_csv', t('Please select valid file.'));
        }
    }
}

//,$lang_code
function photo_check_exist_file_name($fileName) {

    // Check for Data Available in Photo Detail table or not
  $photoDetailQuery = "select pm.photo_id from photo_master as pm inner join photo_detail as pd on(pm.photo_id = pd.photo_id) where pm.photo_filename = '" . $fileName . "' ";
  // and pd.language_code='".$lang_code."'

	$photoDetailResultQuery = db_query($photoDetailQuery)->fetchAssoc();
    return $photoDetailResultQuery;
}

function photo_import_add_form_submit($form_id, $form_values) {
    global $user, $base_path;

    $language = $form_values['values']['language'];

    // Copy File to Local
    $target_path = $_SERVER['DOCUMENT_ROOT'] . base_path() . 'sites/all/modules/ghp_photos/';
    $photo_target_path = $target_path . basename($_FILES['files']['name']['photo_csv']);
    
    @chmod($photo_target_path, '0777');

    if (@move_uploaded_file($_FILES['files']['tmp_name']['photo_csv'], $photo_target_path)) {
        
    } else {
        
    }

    $row = 1;
    $handle = fopen($photo_target_path, "r");
    while (($data = fgetcsv($handle, 1000000, ",")) !== FALSE) {
        $num = count($data);
        
        if ($row != 1 && (trim($data[0]) != '')) {
            
            // ,$form_values['values']['language']
            $photoResultData = photo_check_exist_file_name(mysql_escape_string(trim($data[0])));
            
            $photoEntry = array('photo_filename' => mysql_escape_string(trim($data[0])),
                'category_id' => ((trim($data[1]) != '') ? trim($data[1]) : 0),
                'h_site_id' => ((trim($data[2]) != '') ? trim($data[2]) : null),
								'copyright_id' => ((trim($data[18]) != '') ? trim($data[18]) : 0),
                'photo_display_date' => mysql_escape_string(trim($data[4])),
                'uploaded_by' => $user->uid,
                'uploaded_date' => date('Y-m-d'),
                'modified_date' => date('Y-m-d H:i:s'),
                'photo_created_by' => $user->uid,
                'created_by' => $user->uid,
                'photo_latitude' => mysql_escape_string(trim($data[5])),
                'photo_longitude' => mysql_escape_string(trim($data[6])),
                'photo_publish_status' => 'Y',
                'photo_approve_status' => 'Y'
            );

            if (trim($data[3]) != '') {
                /*
                $d = new DateTime($data[3]);
                $photoDate = $d->format("Y-m-d");
                $photoEntry = array_merge($photoEntry, array('photo_date' => $photoDate));
                */
                $photoEntry = array_merge($photoEntry, array('photo_date' => trim($data[3])));
            }

            if((!empty($photoEntry['photo_date'])) && $photoEntry['photo_date'] != "0000-00-00")
            {             
                $photoEntry['photo_date'] =  mysql_escape_string($photoEntry['photo_date']);
            }
            else
            {
                $photoEntry['photo_date'] = NULL;
            }
            
            if(trim($data[16]) != '')
            {    
                $photo_order = mysql_escape_string(trim($data[16]));
            }
            else
            {
                $photo_order = 0;
            }
            $photoEntry['photo_order_number'] = $photo_order;
            
            // echo "<pre>";
            // echo " photoResultData : ";
            // print_r($photoResultData);
            // print_r($data);
            // echo "</pre>";
            // exit;

            if (empty($photoResultData['photo_id'])) {
                // START INSERT DATA IN PHOTO MASTER
                try {
                    $photo_id = db_insert('photo_master')
                            ->fields($photoEntry)
                            ->execute();
                } catch (Exception $e) {
                    drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
                    
                }
                // STOP INSERT DATA IN PHOTO MASTER                
            } else {
                try {
                    // START UPDATE ENTRY IN PHOTO MASTER TABLE        
                    $updateResult = db_update('photo_master')
                            ->fields($photoEntry)
                            ->condition('photo_id', $photoResultData['photo_id'])
                            ->execute();
                    // STOP UPDATE ENTRY IN PHOTO MASTER TABLE  

                    $photo_id = $photoResultData['photo_id'];
                } catch (Exception $e) {
                    drupal_set_message(t('db_update failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
                }
            }

            $photoDetailEntry = array('photo_id' => $photo_id,
                'language_code' => $language,
                'photo_title' => mysql_escape_string($data[7]),
								'photo_alter_text' => mysql_escape_string($data[19]),
                'photo_description' => mysql_escape_string($data[8]),
                'photo_place' => mysql_escape_string(trim($data[9])),
                'photo_people' => mysql_escape_string(trim($data[10])),
                'photo_source' => mysql_escape_string(trim($data[17])),
                'photo_significance' => mysql_escape_string(trim($data[11])),
                'photographer_name' => mysql_escape_string(trim($data[12])),
                'photo_meta_keywords' => mysql_escape_string($data[13]),
                'photo_meta_description' => mysql_escape_string($data[14]),
            );

            $countPhotoDetail = get_photo_detail_exist($language, $photo_id);

            if ($countPhotoDetail == 0) {
                try {
                    // START ENTRY IN PHOTO DETAIL TABLE        
                    $photo_detail_id = db_insert('photo_detail')
                            ->fields($photoDetailEntry)
                            ->execute();
                    // STOP ENTRY IN PHOTO DETAIL TABLE  
                } catch (Exception $e) {
                    drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
                }
            } else {
                // START UPDATE ENTRY IN PHOTO DETAIL TABLE
                try {
                    $photoUpdateResult = db_update('photo_detail')
                            ->fields($photoDetailEntry)
                            ->condition('photo_id', $photo_id)
                            ->condition('language_code', $language)
                            ->execute();
                } catch (Exception $e) {
                    drupal_set_message(t('db_update failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
                }
                // STOP UPDATE ENTRY IN PHOTO DETAIL TABLE                
            }

            if (mysql_escape_string(trim($data[15])) != '') {

                $photoTagArray = explode(',', mysql_escape_string(trim($data[15])));

                if (count($photoTagArray) > 0) {
                    // REMOVE OLD RECORDS FOR THAT TAGS
                    if (!empty($photo_id)) {
                        $result = db_delete('photo_tags')
                                ->condition('photo_id', $photo_id)
                                ->execute();
                    }

                    foreach ($photoTagArray as $photoTag) {
                        $photoTagEntry = array('photo_id' => $photo_id,
                            'created_date' => date('Y-m-d'),
                            'modified_date' => date('Y-m-d'),
                            'tag_name' => $photoTag,
                        );

                        // START ENTRY IN PHOTO TAG TABLE
                        try {
                            $photo_tag_id = db_insert('photo_tags')
                                    ->fields($photoTagEntry)
                                    ->execute();
                        } catch (Exception $e) {
                            $errorFlag = true;
                            drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
                        }
                        // STOP ENTRY IN PHOTO TAG TABLE
                    }
                }
            }
        }
        $row++;
    }

    fclose($handle);

    // Remove File from Local    
    @unlink($photo_target_path);

    drupal_set_message(t("Photo data has been imported successfully."));
    drupal_goto("admin/ghp_photos/photo_import");
}


/* photo other reference csv import */

function photo_reference_import_add_form() {
    
    global $base_url;
    $languageArray = get_languages();
    
    $adminRenamePath = ((variable_get('rename_admin_path') != NULL) ? variable_get('rename_admin_path_value') : 'admin');

    $form = array('#attributes' => array('enctype' => "multipart/form-data", "class" => "ghpForm"));

    $form['photo_reference_csv'] = array(
        '#type' => 'file',
        '#title' => t('Upload Photo Other Reference: <span title="This field is required." class="form-required">*</span>'),
        '#description' => t('Allowed file .csv only.').'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="'.$base_url.'/'.$adminRenamePath.'/download_sample_csv/photo_reference_content_import">Download Sample CSV</a>',
        '#size' => 20,
    );

    $form['submit'] = array('#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}

function photo_reference_import_add_form_validate($form_id, $form_values){
    
    $allowedExtensions = array("text/plain", "application/octet-stream", "text/comma-separated-values", "text/csv", "application/csv", "application/force-download", "text/html", "application/x-csv", "application/vnd.ms-excel");
    
    if (empty($_FILES['files']['name']['photo_reference_csv'])) {
        form_set_error('photo_reference_csv', t('Upload Photo Reference Content field is required.'));
    } else {
        if (!in_array($_FILES['files']['type']['photo_reference_csv'], $allowedExtensions)) {
            form_set_error('photo_reference_csv', t('Please select valid file.'));
        }

        if (!in_array(mime_content_type($_FILES['files']['tmp_name']['photo_reference_csv']), $allowedExtensions)) {
            form_set_error('photo_reference_csv', t('Please select valid file.'));
        }
    }
    
}
function photo_reference_import_add_form_submit($form_id, $form_values){
    
    global $user, $base_path;

       // Copy File to Local
    $target_path = $_SERVER['DOCUMENT_ROOT'] . base_path() . 'sites/all/modules/ghp_photos/';
    $photo_target_path = $target_path . basename($_FILES['files']['name']['photo_reference_csv']);
    
    @chmod($photo_target_path, '0777');

    if (@move_uploaded_file($_FILES['files']['tmp_name']['photo_reference_csv'], $photo_target_path)) {
        
    } else {
        
    }
    $reference_insert_cnt = $reference_update_cnt = $reference_skip_cnt = 0;
    $skiped_records = array();
    $row = 1;
    $handle = fopen($photo_target_path, "r");
    while (($data = fgetcsv($handle, 1000000, ",")) !== FALSE) {
        $num = count($data);
        if ($row != 1) {
            
            if((trim($data[0]) != '')){
                $photo_id = $data[0];
            }

            $section_name = mysql_escape_string(ucfirst($data[1]));
            $section_format = mysql_escape_string(trim($data[2]));

            // check ref section_format validation 
			module_load_include('module', 'ghp_references', 'ghp_references'); // load ghp_references_sectionformat_validation() function from ghp_references.module file
            $section_format_validation = ghp_references_sectionformat_validation($section_format, $section_name);

			//check ref exist or not
            $countPhotootherreff= get_photo_other_referenc_exist($photo_id,$section_name);
            $photoOtherRefEntry = array(
                'ref_id' => $photo_id,
                'module_name' => 'Photo',
                'section_name' => $section_name,
                'section_format' => $section_format,
            );
          
          
            if($section_format_validation == 'valid')
			{
				if ($countPhotootherreff == 0) {
					try {
						// START ENTRY IN PHOTO DETAIL TABLE        
						$photo_detail_id = db_insert('photo_references')
								->fields($photoOtherRefEntry)
								->execute();
								 $reference_insert_cnt++;
						// STOP ENTRY IN PHOTO DETAIL TABLE  
					} catch (Exception $e) {
						drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
					}
				} else {
					// START UPDATE ENTRY IN PHOTO DETAIL TABLE
					try {
						$photoUpdateResult = db_update('photo_references')
								->fields($photoOtherRefEntry)
								->condition('ref_id', $photo_id)
								->condition('section_name', ucfirst($data[1]))
								->execute();
								$reference_update_cnt++;
					} catch (Exception $e) {
						drupal_set_message(t('db_update failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
					}
					// STOP UPDATE ENTRY IN PHOTO DETAIL TABLE                
				}
			}
			else
			{
                $reference_skip_cnt++;
                $skiped_records[] = $row;
			}
            
        }
        $row++;
    }
    
    fclose($handle);
    
    // Remove File from Local    
    @unlink($photo_target_path);
	
    $import_status_message = $reference_insert_cnt." Photo Reference Inserted, ".$reference_update_cnt." Photo Reference Updated and ".$reference_skip_cnt." Photo Reference are Skipped.";
    if($reference_skip_cnt > 0)
    {
        $import_status_message .= " (".trim(implode(", ", $skiped_records), ", ")." records Skipped)";
    }

    if( $reference_insert_cnt > 0 || $reference_update_cnt > 0)
    {
        drupal_set_message(t("Data Imported Successfully. ".$import_status_message));
    }
    else
    {
        drupal_set_message(t("Data Import Failed. ".$import_status_message),'error');
    }
			
    //drupal_set_message(t("Photo data has been imported successfully."));
    drupal_goto("admin/ghp_photos/photo_reference_import");
    
}

/* End of the other reference */


function ghp_all_photos_rss()
{
//echo "inside photos rss";
$photo_path = variable_get('ghp_photos_path'); 
//echo $photo_path; exit;
global $base_url;
global $language;
 $defaultlan = language_default();
    $trans_url = $base_url;
    if($language->language != $defaultlan->language) { 
        $trans_url .= "/".$language->language;
    }   
$photoRange = db_select('photo_master', 'pm');
    $photoRange->fields('pm');
    $photoRange->addExpression('YEAR(pm.photo_date)', 'photoyear');
    $photoRange->join('photo_detail', 'pd', 'pm.photo_id =  pd.photo_id');
    $photoRange->join('photo_category', 'pc', 'pm.category_id =  pc.category_id');
    $photoRange->fields('pd');
    $photoRange->condition('pd.language_code', $language->language);
    $photoRange->condition('pm.photo_publish_status', 'Y');
    $photoRange->condition('pm.photo_approve_status', 'Y');
	$photoRange->orderBy('pc.category_id', 'ASC');
    $photoRange->orderBy('pm.photo_order_number ', 'ASC');
	$photolistarray = (array)$photoRange->execute()->fetchAll();
	//echo "<pre>";
	//print_r($photolistarray);
	//exit;

$output ='<?xml version="1.0" encoding="UTF-8" ?>
<rss version="2.0">
<channel>
  <title>PHOTOS</title>';

		foreach($photolistarray as $rssphotolist) {
		$title = $rssphotolist->photo_title;
		$photo = $rssphotolist->photo_filename;
		$description = $rssphotolist->photo_description;
		$place = $rssphotolist->photo_place;
		$source = $rssphotolist->photo_source;
		$photographername = $rssphotolist->photographer_name;
		$link = $trans_url.'/photosdetail/'.base64_encode($rssphotolist->photo_id);
 
  $output .='<item>
    <title><![CDATA['.$title.']]></title>
	<link>'.$link.'</link>
	<description><![CDATA[<div><a href="'.$link.'"><img width="250" src="'.$photo_path.$photo.'"></a></div><div>'.$description.'</div><div>'.$place.'</div><div>'.$source.'</div><div>'.$photographername.'</div>]]></description>
  </item>';
 } 
$output .='</channel>
</rss>';
echo $output;
exit;
}

function ghp_photos_rss()
{
global $base_url;
global $language;
 $defaultlan = language_default();
    $trans_url = $base_url;
    if($language->language != $defaultlan->language) { 
        $trans_url .= "/".$language->language;
    }   
$photo_path = variable_get('ghp_photos_path'); 
$photoid = filtertext(strip_tags(base64_decode(arg(2))));
global $base_url;
global $language;

    $resultimg = db_query("SELECT pm.photo_id,pm.photo_filename,pd.photo_title, pd.photo_description, pd.photo_place, pd.photo_source, pd.photo_people, pd.photographer_name FROM photo_master as pm inner join photo_detail as pd on(pd.photo_id = pm.photo_id) WHERE pm.photo_id='" . $photoid . "' AND pd.language_code='" . $language->language . "'");
	$photolistarray = $resultimg->fetchAll();
	//echo "<pre>";
	//print_r($photolistarray);
	//$cnt = count($authorbooksresult);
	//echo $cnt;
	//exit;
	
$output ='<?xml version="1.0" encoding="UTF-8" ?>
<rss version="2.0">
<channel>
  <title>PHOTOS</title>';

		foreach($photolistarray as $rssphotolist) {
		$title = $rssphotolist->photo_title;
		$photo = $rssphotolist->photo_filename;
		$description = $rssphotolist->photo_description;
		$place = $rssphotolist->photo_place;
		$source = $rssphotolist->photo_source;
		$photographername = $rssphotolist->photographer_name;
		$link = $trans_url.'/photosdetail/'.base64_encode($rssphotolist->photo_id);
 
  $output .='<item>
    <title><![CDATA['.$title.']]></title>
	<link>'.$link.'</link>
	<description><![CDATA[<div><a href="'.$link.'"><img width="250" src="'.$photo_path.$photo.'"></a></div><div>'.$description.'</div><div>'.$place.'</div><div>'.$source.'</div><div>'.$photographername.'</div>]]></description>
  </item>';
 } 
$output .='</channel>
</rss>';
echo $output;
exit;
}