<?php

function ghp_photocategory_entry_load($entry = array()) {
    
    if (!empty($_REQUEST['languagename'])) {
        $languageid = $_REQUEST['languagename'];
    } else {
        $languageid = "en";
    }
  
    $select = db_select('photo_category', 'pc')->extend('PagerDefault')->limit(50);;
    $select->fields('pc', array('category_id', 'category_order'));
    $select->fields('pcd', array('category_name', 'language_code'));
    $select->fields('lan', array('name'));
    $select->leftJoin('photo_category_detail', 'pcd', 'pc.category_id = pcd.category_id');
	$select->leftJoin('languages', 'lan', 'pcd.language_code = lan.language');
	$select->condition('pcd.language_code', $languageid);
    $select->orderBy('category_order', 'ASC');

    // Return the result in object format.
    return $select->execute()->fetchAll();
}

function ghp_photocategory_list() {

    global $base_url;
    
	$adminRenamePath = ((variable_get('rename_admin_path') !=  NULL) ? variable_get('rename_admin_path_value') : 'admin'); 
    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/ghp_photocategory.js');
    //drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/photo_category.js');
       
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
    
    
    $output .= '<p><a href="'.$base_url.'/'.$adminRenamePath.'/photo_category/add">Add Category</a></p>';
    
    $output .= '<form name="frmlang" method="post" action="photo_category">';
    $output .= '<input type="hidden" name="languageId" id="languageId" value="' . ((isset($_REQUEST['languagename'])) ? $_REQUEST['languagename'] : 'en') . '" />';
	$output .='<input type="hidden" name="baseurlpath" id="baseurlpath" value="' . $base_url . '" />';
    $output .='<input type="hidden" name="adminRenamePath" id="adminRenamePath" value="' . $adminRenamePath . '" />';
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


    // Get all entries in the photo category table.
    if ($entries = ghp_photocategory_entry_load()) {

        $output .= '<form name="frmGHPphotoCategory" method="post" action="'.$base_url.'/'.$adminRenamePath.'/photo_category/delete"><input type="hidden" name="del_id" id="del_id" value="" /><input type="hidden" name="lang_code" id="lang_code" value="' . ((isset($_REQUEST['languagename'])) ? $_REQUEST['languagename'] : 'en') . '" />';

        $rows = array();
		$i=1;
        foreach ($entries as $key => $entry) {
            
            $entry->edit_link = '<a href="'.$base_url.'/'.$adminRenamePath.'/photo_category/add/'.$entry->language_code . '/' . $entry->category_id . '" title="Edit Photo Category">Edit</a>';

           $entry->delete_link = '<a href="#" onclick=delete_photocategory("' . $entry->category_id . '","' . $entry->language_code . '") title="Delete Category">Delete</a>';
		   
           $entry->category_id = '<input type="checkbox" class="selectcategorycheck" value="'.$entry->category_id.'" name="categoryselect[]" id="checkbox'.$i.'" /> ';

            $rows[$key] = (array) $entry;
			$i++; 
            //unset($rows[$key]['category_id']);
            unset($rows[$key]['language_code']);
        }
				
		// Make a table for them.
        $table_attributes = array('id' => 'tableStructure1');
		$header = array('<input type="checkbox" name="photocategorycheckall[]" class = "photocategorycheck" onclick="toggleCheckboxes()" id="photocategorycheckall" />',t('Photo Category Order Number'), t('Photo Category Name'), t('Language'), t('Edit'), t('Delete'));
        //$header = array(t('Category Name'), t('Language'), t('Category'), t('Edit'), t('Delete'));
        //$header = array(t('Category ID'), t('Category Order Number'), t('Category Name'), t('Edit'));
		$output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => $table_attributes));
        $output .= theme('pager', array('tags' => array(), 'parameters' => $parameters));
        $output .='<input type="button" class="btnbg" name="deletephotocategory" onclick="return deleteall_photocategory(\'Deleteall\');" value="Delete" />';
        $output .= '</form>';
    } else {
        drupal_set_message(t('No entries have been added yet.'));
    }
    return $output;
}

function photocategory_delete() {

    $id = check_plain($_POST["del_id"]);
    $lang_code = check_plain($_POST["lang_code"]);

    if (isset($id) && $id != '') {

        // DELETE FROM TRANSLATION TABLE
        $result = db_delete('photo_category_detail')
                ->condition('category_id', $id)
                ->condition('language_code', $lang_code)
                ->execute();

        // START FOR RECORD COUNT
        $categoryCountQuery = "select * from photo_category_detail where category_id = " . $id . "";
        $categoryCountResultQuery = db_query($categoryCountQuery);
        $countCategories = $categoryCountResultQuery->rowCount();
        // END FOR RECORD COUNT        

        if ($countCategories == 0) {
            $categoryResult = db_delete('photo_category')
                    ->condition('category_id', $id)
                    ->execute();
        }

        drupal_set_message(t("Photo Category has been deleted successfully."));
    } else {
        drupal_set_message(t('Invalid photo category Id.'));
    }
	
        drupal_goto("admin/ghp_photos/photo_category");
   
}

function photocategory_deleteall()
{
	$lang_code = check_plain($_POST["lang_code"]);

         if (isset($lang_code) && $lang_code != '') {
            if (!empty($_POST['categoryselect'])) {
                $stateids = $_POST['categoryselect'];
                 foreach($stateids as $row){
                // DELETE FROM TRANSLATION TABLE
                $result = db_delete('photo_category_detail')
                        ->condition('category_id', $row)
                        ->condition('language_code', $lang_code)
                        ->execute();

                // START FOR RECORD COUNT
                $categoryCountQuery = "select * from photo_category_detail where category_id = " . $row . "";
                $categoryCountResultQuery = db_query($categoryCountQuery);
                $countCategories = $categoryCountResultQuery->rowCount();
                // END FOR RECORD COUNT        

                if ($countCategories == 0) {
                    $categoryResult = db_delete('photo_category')
                            ->condition('category_id', $row)
                            ->execute();
                }
            }
             drupal_set_message(t("Photo Category has been deleted successfully."));
        } 
        else
        {
             drupal_set_message(t("No photo category found to delete."));
        }    
	 }
    else
    {
        drupal_set_message(t("No photo category language found to delete."));
    } 
 
     drupal_goto("admin/ghp_photos/photo_category");
}

function get_photocategory_data($lang_code, $photocategoryId) {

    $select = db_select('photo_category', 'pc');
    $select->fields('pc', array('category_order'));
    $select->fields('pcd', array('category_name', 'category_description', 'language_code'));
    $select->leftJoin('photo_category_detail', 'pcd', 'pc.category_id = pcd.category_id');
	//$select->leftJoin('languages', 'lan', 'ct.language_code = lan.language');
	$select->condition('language_code', $lang_code);
    $select->condition('pc.category_id', $photocategoryId);
    $results = $select->execute()->fetchAssoc();

    return $results;
}

function get_all_photocategory_data($lang_code) {

    $select = db_select('photo_category', 'pc');
    $select->fields('pc', array('category_id'));
    $select->fields('pcd', array('category_name','language_code'));
    $select->leftJoin('photo_category_detail', 'pcd', 'pc.category_id = pcd.category_id');
	//$select->leftJoin('languages', 'lan', 'ct.language_code = lan.language');
	$select->condition('language_code', $lang_code);
    $results = $select->execute()->fetchAll();

    return $results;
}

function check_exist_photocategory($language, $photocategoryName, $photocategoryId = '') {

    $select = db_select('photo_category', 'pc');
    $select->fields('pc', array('category_id'));
    $select->leftJoin('photo_category_detail', 'pcd', 'pc.category_id = pcd.category_id');
    $select->condition('category_name', $photocategoryName);

    if (!empty($photocategoryId)) {
        $select->condition('pc.category_id', $photocategoryId, '!=');
    }

    $results = $select->execute()->fetchAll();
    return count($results);
}

function check_photocategory_data_exist($photocategoryId,  $langCode) {

    $select = db_select('photo_category', 'pc');
    $select->fields('pc', array('category_id'));
    $select->leftJoin('photo_category_detail', 'pcd', 'pc.category_id = pcd.category_id');
	$select->condition('language_code', $langCode);
    $select->condition('pc.category_id', $photocategoryId);

    $results = $select->execute()->fetchAll();

    return count($results);
}

function ghp_photocategory_add_form() {
    global $base_url;

    drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/ghp_photocategory_addedit.js');
    
    $adminRenamePath = ((variable_get('rename_admin_path') !=  NULL) ? variable_get('rename_admin_path_value') : 'admin');

    $lang_code = arg(3);
    $photocategoryId = arg(4);

    //$categoryLanguage = '';
	$categoryName = '';
    $categoryDescription = '';
    $categoryOrder = '';
    
    if (!empty($lang_code) && !empty($photocategoryId)) {
        // GET DATA FROM LANGUAGE CODE & PHOTO CATEGORY ID
        $categoryDataArray = get_photocategory_data($lang_code, $photocategoryId);

        //$categoryLanguage = $categoryDataArray['language_code'];;
        $categoryName = $categoryDataArray['category_name'];
        $categoryDescription = $categoryDataArray['category_description'];
        $categoryOrder = $categoryDataArray['category_order'];
    }
    elseif (!empty($lang_code) && empty($photocategoryId)) {
        $lang_code = $lang_code;
    } else {
        $lang_code = 'en';
    }
    
    $languageArray = get_languages();

    $form = array('#attributes' => array("class" => "ghpForm"));

    $form['baseUrl'] = array(
        '#type' => 'hidden',
        '#default_value' => $base_url . '/'.$adminRenamePath.'/photo_category/add/',
        '#attributes' => array('id' => 'baseUrl')
    );

    $form['photocategoryId'] = array(
        '#type' => 'hidden',
        '#default_value' => $photocategoryId,
        '#attributes' => array('id' => 'photocategoryId')
    );
    
    $form['language'] = array('#type' => 'select',
        '#title' => t('Language'),
        '#options' => $languageArray,
        '#default_value' => $lang_code,
    );
  
    $form['category_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Category Name'),
        '#size' => 100,
        '#maxlength' => 255,
        '#default_value' => $categoryName,
        '#required' => TRUE,
    );

    $form['category_description'] = array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#cols' => 10,
        '#rows' => 5,
        '#maxlength' => 500,
        '#required' => FALSE,
        "#resizable" => FALSE,
        '#default_value' => $categoryDescription,
    );
    
    $form['category_order'] = array(
        '#type' => 'textfield',
        '#title' => t('Category Order Number'),
        '#size' => 5,
        '#maxlength' => 5,
        '#required' => TRUE,
        '#default_value' => $categoryOrder,
    );

    $form['submit'] = array('#type' => 'submit',
        '#value' => t('Save'),
    );
    return $form;
}

function ghp_photocategory_add_form_validate($form_id, $form_values) {

    //$language = $form_values['values']['language'];
	$category_name = $form_values['values']['category_name'];
    $category_description = $form_values['values']['category_description'];
    $category_order = $form_values['values']['category_order'];
    /* Fetch regular expression value from general setting Added by Maya*/
    $allowed_characters = variable_get_value('ghp_reg_exp');    
       
    /* Fetch not allowed regular expression to match for hindi and gujarati language Added by Maya*/
    $not_allowed_characters = get_not_allowed_characters();
    $not_allowed_characters = str_replace("\\", "\\\\", $not_allowed_characters);
    $not_allowed_characters = str_replace("/", "\/", $not_allowed_characters);
    $pattern = preg_quote($not_allowed_characters, '#');
    
	if (!empty($category_name)) {
        if (hasHtml($category_name)) {
            form_set_error('category_name', t('Please enter valid category name'));
        } else {
            //if (!preg_match("/^([a-zA-Z0-9.\,\- ])+$/", $category_name)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $category_name)) {
            if (preg_match("#[{$pattern}]#", $category_name)) {
                form_set_error('category_name', t('Please enter valid photo category name. Allowed special characters are '.$allowed_characters));
            }
        }
    }

    if (!empty($category_description)) {
        if (hasHtml($category_description)) {
            form_set_error('category_description', t('Please enter valid description'));
        } else {
            //if (!preg_match("/^([a-zA-Z0-9.\,\- ])+$/", $category_description)) {
            //if (!preg_match("/^[ A-Za-z0-9\,".$allowed_characters."]*$/", $category_description)) {
            if (preg_match("#[{$pattern}]#", $category_description)) {    
                form_set_error('category_description', t('Please enter valid description. Allowed special characters are '.$allowed_characters));
            }
        }
    }
    
    if (!empty($category_order)) {
        if (hasHtml($category_order)) {
            form_set_error('$category_order', t('Please enter valid photo category Order'));
        } else {
            if (!preg_match("/^([0-9])+$/", $category_order)) {
                form_set_error('$category_order', t('Please enter valid photo category Order'));
            }
        }
    }

    $categoryExist = check_exist_photocategory($form_values['values']['language'], $form_values['values']['category_name'], $form_values['values']['photocategoryId']);

    if ($categoryExist > 0) {
        form_set_error('category_name', t('Photo Category name already exist.'));
    }
}

function ghp_photocategory_add_form_submit($form_id, $form_values) {
    
    global $user;
    
    if (empty($form_values['values']['photocategoryId'])) {
        // START ENTRY IN MASTER TABLE

        //$entry = array('category_order' => mysql_escape_string($form_values['values']['category_order']),'created_by' => $user->uid, 'created_date' => date('Y-m-d'));
   
           // $slug = str_replace(' ', '-', strtolower(strtoupper($form_values['values']['category_name'])));
            $entry = array('category_order' => $form_values['values']['category_order'],'created_by' => 1, 'created_date' => date('Y-m-d'));
      
		
        $category_id = db_insert('photo_category')
                ->fields($entry)
                ->execute();
         // STOP ENTRY IN MASTER TABLE    
        try {
            
            $DetailEntry = array('category_id' => $category_id, 'language_code' => 'en', 'category_name' => mysql_escape_string($form_values['values']['category_name']), 'category_description' => mysql_escape_string($form_values['values']['category_description']));
            
            // START ENTRY IN RELATION TABLE
            $t_category_id = db_insert('photo_category_detail')
                    ->fields($DetailEntry)
                    ->execute();
         
            drupal_set_message(t("Photo Category added successfully."));
			drupal_goto("admin/ghp_photos/photo_category");
        } catch (Exception $e) {
            drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
        // STOP ENTRY IN MASTER TABLE    
    } else {
        $category_id = $form_values['values']['photocategoryId'];

        $updateMasterArray = array('category_order' => mysql_escape_string($form_values['values']['category_order']),'created_by' => $user->uid);
		
  
           $updateMasterArray = array('category_order' => mysql_escape_string($form_values['values']['category_order']), 'created_by' => 1);
      
            $updateMasterArray = array('category_order' => mysql_escape_string($form_values['values']['category_order']), 'created_by' => 1);

		
        $updateRecord = db_update('photo_category')
                ->fields($updateMasterArray)
                ->condition('category_id', $category_id)
                ->execute();
    
        //$DetailEntry = array( 'language_code' => mysql_escape_string($form_values['values']['language']),'category_name' => mysql_escape_string($form_values['values']['category_name']), 'category_description' => mysql_escape_string($form_values['values']['category_description']));
        
		 // CHECK FOR DATA EXIST IF  YES THEN UPDATE ELSE INSERT
		$existCount = check_photocategory_data_exist($category_id, $form_values['values']['language']);

		$tEntry = array('category_id' => $category_id, 'language_code' => $form_values['values']['language'], 'category_name' => mysql_escape_string($form_values['values']['category_name']), 'category_description' => mysql_escape_string($form_values['values']['category_description']));
	
		 if ($existCount == 0) {
        try {
            // START ENTRY IN RELATION TABLE
            $t_category_id = db_insert('photo_category_detail')
                    ->fields($tEntry)
                    ->execute();
            // STOP ENTRY IN RELATION TABLE    
            drupal_set_message(t("Photo Category added successfully."));
        } catch (Exception $e) {
            drupal_set_message(t('db_insert failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
    } else {
        try {
            // START UPDATE ENTRY IN RELATION TABLE        
            $updateResult = db_update('photo_category_detail')
                    ->fields($tEntry)
                    ->condition('category_id', $tEntry['category_id'])
                    ->condition('language_code', $tEntry['language_code'])
                    ->execute();
            // STOP UPDATE ENTRY IN RELATION TABLE     

            drupal_set_message(t("Photo Category updated successfully."));
        } catch (Exception $e) {
            drupal_set_message(t('db_update failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
    }
         drupal_goto("admin/ghp_photos/photo_category");
    
}
}



        /* try {
            // START UPDATE ENTRY IN RELATION TABLE        
            $updateResult = db_update('photo_category_detail')
                    ->fields($DetailEntry)
                    ->condition('category_id', $form_values['values']['photocategoryId'])
                    ->execute();
            // STOP UPDATE ENTRY IN RELATION TABLE     

            drupal_set_message(t("Photo Category updated successfully."));
        } catch (Exception $e) {
            drupal_set_message(t('db_update failed. Message = %message, query= %query', array('%message' => $e->getMessage(), '%query' => $e->query_string)), 'error');
        }
        
    }
    drupal_goto("admin/ghp_photos/photo_category");
}*/