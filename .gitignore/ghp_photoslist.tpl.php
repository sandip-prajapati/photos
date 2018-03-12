<?php
global $base_path;
global $base_url;
global $language;
drupal_add_css(drupal_get_path('module', 'ghp_tributes') . '/css/tributestogandhiji.css');
$ghp_data_root_path = variable_get('ghp_data_root_path');
$templateImgPath = $base_url . '/sites/all/themes/ghp/images/';
$rootImgPath = $base_url . '/sites/default/files/';
$graphicsFolderUrl = $base_url.'/sites/all/modules/ghp_chronology/images/graphics/';
$photoImageUrl = $base_url . "/" . drupal_get_path('module', 'ghp_photos') . '/images/';
// Make next and previous pagination here
//print_r($_SESSION['photoTotalIds']);
if (isset($_SESSION['photoTotalIds']) && $_SESSION['photoTotalIds'] != '') {
    $key = array_search(base64_decode(arg(1)), $_SESSION['photoTotalIds']);
    $next = @$_SESSION['photoTotalIds'][$key + 1];
    $prev = @$_SESSION['photoTotalIds'][$key - 1];
} else {
    $next = '';
    $prev = '';
}


?>
<link rel="stylesheet" href="/resources/demos/style.css">
<!-- <script type="text/javascript" src="//code.jquery.com/ui/1.11.2/jquery-ui.js"></script> -->
<?php
//drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery-1.4.3.min.js');
//drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery-ui.js');
//drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.ui.core.js');
//drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery.ui.widget.js');
//drupal_add_js(drupal_get_path('module', 'ghp_photos') . '/js/jquery-ui-accordian.js');
//drupal_add_css(drupal_get_path('module', 'ghp_photos') . '/css/jquery-ui.css');

$get_photo_path = variable_get('ghp_photos_path');
$loaderImagePath = $base_url . '/' . drupal_get_path('module', 'ghp_tributes') . '/images/';
$display_photos_of = arg(1);
$display_photo_reference_id = arg(2);

$seturl = '';
$defaultlan = language_default();
if ($display_photos_of != '' && $display_photo_reference_id != '') {
    $seturl = $display_photos_of . "/" . $display_photo_reference_id;
}

$photo_url = $get_photo_path;
$photo_small_url = $get_photo_path . 'small/';
$photo_medium_url = $get_photo_path . 'medium/';

$photoImagePath = drupal_get_path('module', 'ghp_photos') . '/images/';
$photoViewUrl = url($photoImagePath, array('absolute' => true));

$photoModuleToken = md5(microtime());
$_SESSION['photoModuleToken'] = $photoModuleToken;


// GET PHOTO CATEGORY LIST
$select = db_select('photo_category', 'c');
$select->fields('c', array('category_id', 'category_order'));
$select->fields('ct', array('category_name'));
$select->condition('ct.language_code', $language->language);
$select->leftJoin('photo_category_detail', 'ct', 'c.category_id = ct.category_id');
$select->orderBy('category_order', 'ASC');
$photoCategoryList = $select->execute()->fetchAll();
?>

<!--[if IE 6]>
<script src="<?php echo drupal_get_path('module', 'ghp_photos') . '/js/DD_belatedPNG_0.0.8a.js'; ?>" type="text/javascript"></script>
<script type="text/javascript">
DD_belatedPNG.fix('body *');
</script>
<![endif]-->


<?php
$defaultlan = language_default();
$trans_url = $base_url;
if ($language->language != $defaultlan->language) {
    $trans_url .= "/" . $language->language;
}
?>    
<form name="photoSearch" id="photoSearch" action="<?php echo $trans_url . '/photos-of-mahatma-gandhi'; ?>" method="post">
<?php
if (isset($_POST['photoResetBtn']) == 'Reset') {
    $photoKeyword1 = $_POST['photoKeyword1'] = '';
    $photoKeyword = $_POST['photoKeyword'] = '';
    $selectedCatID = '';
}
else
{
    $photoKeyword1 = (isset($_POST['photoKeyword1']) ? filtertext(strip_tags($_POST['photoKeyword1'])) : '');
    $photoKeyword = (isset($_POST['photoKeyword']) ? filtertext(strip_tags($_POST['photoKeyword'])) : '');
}
    
    if($global_search_keyword != ''){
        $photoKeyword1 = $global_search_keyword;
        $photoKeyword = $global_search_keyword;
        unset($_SESSION['header_search_keyword']);
    }
    
?>
    <div id='GalleryLoaderDisplay' style=" display: none; left: 50%; top: 325px; z-index: 100;">
		<img src="<?php echo $loaderImagePath . 'loading.gif'; ?>" alt='Loading' /></div>
    <input type="hidden" name="norecord" id="norecord" value="<?php echo t('No Record Found.'); ?>" />
    <input type="hidden" name="lastPagingValue" id="lastPagingValue" value="0" />
    <input type="hidden" name="ajaxFlagSet" id="ajaxFlagSet" value="" />
    <div class="photohead photoheadWrap" >
        <div class="midHead">
            <div class="head">
                <!-- <form name="photoSearch" id="photoSearch" action="" method="post"> -->
                <div class="volume"><label for="photo_category"><?php echo t('Category') ?>: </label></div>
                <div class="volumeCount volumeselectdefault volumeselect" style="margin-top: 0px;">
                    <select name="photo_category" class="volumelistcls" id="photo_category">
                        <option value=""><?php echo t('Select Category'); ?></option>
<?php
foreach ($photoCategoryList as $key => $category) {
    ?>
                            <option value="<?php echo $category->category_id; ?>" <?php echo isset($selectedCatID) && $selectedCatID == $category->category_id ? "selected='selected'" : ''; ?>><?php echo t($category->category_name); ?></option>
                            <?php
                        }
                        ?>
                    </select>
                </div>

                <div class="volume"><label for="photoKeyword"><?php echo t('Search Topic') ?></label></div>


                <div id="photoSearchBlock" class="cwmgSearch" style="display: none;">
                    <input type="text" maxlength="25" name="photoKeyword1" id="photoKeyword1" value="<?php echo $photoKeyword1 ?>" />
                </div>
                <div id="photoSearchBlockPhp" class="photoSearchWcag">
                    <input type="text" maxlength="25" name="photoKeyword" id="photoKeyword" value="<?php echo $photoKeyword; ?>" />                    
                </div>                    
                <div>
                    <input type="hidden" name="photoModuleToken" id="photoModuleToken" value="<?php echo $photoModuleToken; ?>" readonly="readonly" />                    
                    <input type="submit" name="photoSearchBtn" id="photoSearchBtn" value="<?php echo t('Search') ?>" title="<?php echo t('Search') ?>" class="cwmgBtn" />
                    <input type="submit" name="photoResetBtn" id='photoResetBtn' value="<?php echo t('Reset'); ?>" title="<?php echo t("Reset") ?>" class="cwmgBtn" />
                </div>
                <div class="volume" id="withjs" style="display:none; " ><?php echo t('Showing') . ": "; ?>
                    <span id="currentcount"><?php echo count($photoListArray); ?></span><?php echo " / "; ?>
                    <span id="totalrecord"><?php echo $totalPhotoCount; ?></span>
<?php echo " " . t('Photos'); ?>
                </div>
                <!-- </form> -->
                <div class="viewbox">
                    <?php /*<a id="rssvolume" target="_blank" href="<?php echo $trans_url . '/photos/rss'; ?>" style="margin: 0px 10px 0px 0px;" class="rssfeed" title="Rss"></a> */ ?>

                    <ul>
                        <li id="printButton" style="display: none;"><a id="printPhotos" href="#printPhotosId" class="printBtnNew" title="<?php echo t('Print'); ?>"><span style="display: none;"><?php echo t('Print Image'); ?></span></a></li>
<?php
$trans_url = $base_url;
if ($language->language != $defaultlan->language) {
    $trans_url .= "/" . $language->language;
}
?>
                        <li><span><?php echo t('Standard View') ?></span>
                            <a href="<?php echo $trans_url; ?>/photos/<?php echo $seturl; ?>" class="listview current" title="<?php echo t('Standard View'); ?>"><span style="display: none;"><?php echo t('Standard View'); ?></span></a></li>
                        <li><span><?php echo t('Thumbnail View'); ?></span><a href="<?php echo $trans_url ?>/photos_thumbview/<?php
                        if (isset($setCatId) != '') {
                            echo $seturl . $setCatId;
                        } else {
                            echo $seturl;
                        }
?>" class="thumbview" title="<?php echo t('Thumbnail View'); ?>"><span style="display: none;"><?php echo t('Thumbview'); ?></span></a></li>

                    </ul>
                </div>
            </div>
        </div>
    </div>  

    <?php if ($photoCount > 0) { ?>
        <div id="booksAllDiv" class="photoheadWrap" style="display: none;">
            <!-- <form name="photoForm" id="photoForm" action="" method="post"> -->
            <div style="display:inline;">
                <input type="hidden" name="currentImgIndex" id="currentImgIndex" value="" />
                <input type="hidden" name="currentImgId" id="currentImgId" value="<?php echo $photoid; ?>" />
                <input type="hidden" name="baseUrl" id="baseUrl" value="<?php echo $base_url; ?>" />
                <input type="hidden" name="photo_download_path" id="photo_download_path" value="<?php echo $ghp_photo_download_path; ?>" />
                <input type="hidden" name="global_search_keyword" id="global_search_keyword" value="<?php echo $global_search_keyword; ?>" />
                <input type="hidden" name="language" id="language" value="<?php echo $language->language; ?>" />
                <input type="hidden" name="photo_url" id="photo_url" value="<?php echo $photo_url ?>" />    
                <input type="hidden" name="photo_small_url" id="photo_small_url" value="<?php echo $photo_small_url ?>" />    
                <input type="hidden" name="photo_medium_url" id="photo_medium_url" value="<?php echo $photo_medium_url ?>" />    
                <input type="hidden" name="paging" id="paging" value="18" />                    <input type="hidden" name="totalRecords" id="totalRecords" value="<?php echo $totalPhotoCount ?>" />
                <input type="hidden" name="selected_checkbox" id="selected_checkbox" value="" />
            </div>                
            <div class="galleryCon">
                <!-- Gallery Start -->
                <div id="gallery" class="ad-gallery">
                    <div class="ad-nav">
                        <div class="ad-thumbs">
                            <ul class="ad-thumb-list" id="photoListUl">
                                <?php
                                foreach ($photoListArray as $key => $photoArray) {
									$smallImage = $ghp_data_root_path . 'ghp_photos/small/' . $photoArray->photo_filename;
									$orgImage = $ghp_data_root_path . 'ghp_photos/' . $photoArray->photo_filename;
									//$smallImage = $photo_small_url . $photoArray->photo_filename;
									$mediumImage = $photo_medium_url . $photoArray->photo_filename;
                                    //$orgImage = $photo_url . $photoArray->photo_filename;
									if (file_exists($smallImage)) {
									 $smallImage = $photo_small_url . $photoArray->photo_filename;
									} else {
									 $smallImage = $photo_medium_url.'photo-gallery-no-img.jpg';
									}
									if (file_exists($orgImage)) {
									 $orgImage = $photo_url . $photoArray->photo_filename;
									} else {
									 $orgImage = $photo_url.'photo-gallery-no-img.jpg';
									}
                                    
                                    

                                    if ($photoArray->photo_alter_text != '') {
                                        $alttext = $photoArray->photo_alter_text;
                                    } else {
                                        $alttext = $photoArray->photo_filename;
                                    }

                                    if ($photoArray->photo_title != '') {
                                        $titletext = $photoArray->photo_title;
                                    } else {
                                        $titletext = $photoArray->photo_filename;
                                    }
                                    ?> 
                                    <li id="gandhiphoto<?php echo $key; ?>">
                                        <a href="<?php echo $orgImage; ?>" ><img id="<?php echo $photoArray->photo_id; ?>" src="<?php echo $smallImage; ?>" title="<?php echo $titletext; ?>" width="42" height="42" alt="<?php echo $alttext; ?>"  class="<?php echo (($key == 0) ? 'image0' : 'image1'); ?>" style="vertical-align: middle;" /></a> 
                                        <input type="hidden" name="year" value="<?php echo $photoArray->photoyear; ?>" />
                                    </li>  

                                <?php } ?>

                            </ul>
                        </div>
                    </div>
                    <div class="ad-image-bg">                    
                        <div class="ad-image-wrapper">
                        </div>
                    </div>
                      
                    <div class="download" id="downloadLink">
                        <a id="photoDownload" href="" title="Download Image"><?php echo t('Download Image'); ?><img src="<?php echo $base_url; ?>/sites/all/themes/ghp/images/bar-download-new-img.png" title="Download" alt="Download" class="newDownIcon"></a> 
                    </div>
                   
                </div>
                <!-- Gallery End -->
                <?php
                if ($display_photos_of != '' && $display_photo_reference_id != '') {
                    ?>
                    <input type="hidden" name="display_photos_url" id="display_photos_url" value="<?php echo $display_photos_of . "/" . $display_photo_reference_id; ?>" />
                    <?php
                }
                ?>
            </div>
            <div class="clearfix"></div>
            <?php foreach ($photoListArray as $key => $photoArray) { ?>
                <div class="gallery_inner" id="photoDescription<?php echo $key; ?>" style="display: <?php echo ($key == 0) ? '' : 'none'; ?>">

                    <?php
                    if (!empty($photoArray->photo_title)) {
                        ?>
                        <h3><?php echo t('Title'); ?>: <span class="phototitle"><?php echo stripslashes($photoArray->photo_title); ?></span></h3>

                        <?php
                    }

                    if (!empty($photoArray->photo_description)) {
                        ?>
                        <h3><?php echo t('Description'); ?>: <span class="photodesc"><?php echo stripslashes($photoArray->photo_description); ?></span></h3>
                        <?php
                    }

                    if (!empty($photoArray->photo_place)) {
                        ?>
                        <h3><?php echo t('Place'); ?>: <span class="photoplace"><?php echo stripslashes($photoArray->photo_place); ?></span></h3> 
                        <?php
                    }

                    if (!empty($photoArray->photo_people)) {
                        ?>
                        <h3><?php echo t('People'); ?>: <span class="photopeople"><?php echo stripslashes($photoArray->photo_people); ?></span></h3>
                        <?php
                    }

                    if (!empty($photoArray->photo_source)) {
                        ?>
                        <h3><?php echo t('Source'); ?>: <span class="photosource"><?php echo stripslashes($photoArray->photo_source); ?></span></h3> 
                        <?php
                    }
                 
                    ?>  

                        
                        
                </div>
            
            
            <?php } ?>

            <input type="hidden" name="phototitle" id="phototitle" value="<?php echo t('Title'); ?>" />
            <input type="hidden" name="photodesc" id="photodesc" value="<?php echo t('Description'); ?>" />
            <input type="hidden" name="photoplace" id="photoplace" value="<?php echo t('Place'); ?>" />
            <input type="hidden" name="photopeople" id="photopeople" value="<?php echo t('People'); ?>" />
            <input type="hidden" name="photosource" id="photosource" value="<?php echo t('Source'); ?>" />    
            <input type="hidden" name="photonxt" id="photonxt" value="<?php echo t('Next'); ?>" />    
            <input type="hidden" name="photoprevious" id="photoprevious" value="<?php echo t('Previous'); ?>" />    
            <!-- </form> -->  

        </div>
        <!--End of js enable -->
        
        
        <div id="booksAllDivPhp"  class="disabledGallery photoheadWrap">
            <!-- <form name="frmPhotos" id="frmPhotos" action="" method="post">  -->    
            <?php
            
           
      
            if(isset($_POST['tab_header']) || isset($_GET['pager']))
            {
                $photoWCAGcounter = isset($_POST['photoWCAGCnt']);
            }
            else
            {
                $photoWCAGcounter = 0;
                if (isset($_POST['nextbutton'])) {
                    $photoWCAGcounter = $_POST['photoWCAGCnt'] + 1;
                } elseif (isset($_POST['prevbutton'])) {
                    $photoWCAGcounter = $_POST['photoWCAGCnt'] - 1;
                } else {
                    $photoWCAGcounter = 0;
                }

                if ($photoWCAGcounter < 0 || ($totalPhotoCount == $photoWCAGcounter)) {
                    $photoWCAGcounter = 0;
                }
            }  
         
            if(isset($_GET['pager']))
            {
                  $activeTabArr = explode('_', $_GET['pager']);
                  $photoWCAGcounter = $activeTabArr[2];
            }
            
            $orgImage = $photo_url . $photoListArray[$photoWCAGcounter]->photo_filename;
            ?>
            <div class="galleryCon">
                <div class="proImg" style="display: inline; margin: 0 0 0 98px;">
                    <input type="hidden" id="photoWCAGCnt" name="photoWCAGCnt" value="<?php echo $photoWCAGcounter ?>" />
                    <input type="hidden" id="CurrenIndex" name="CurrenIndex" value="<?php echo $photoWCAGcounter ?>" />
                    <input type="hidden" id="photoidWOJ" name="photoidWOJ" value="<?php echo $photoListArray[$photoWCAGcounter]->photo_id ?>" />
                    <img src="<?php echo $orgImage; ?>"  alt="<?php echo stripslashes($photoListArray[$photoWCAGcounter]->photo_title); ?>" height="500"  />
                </div>
                <?php
                                $img = $orgImage;
				$mainImg = '';
                                $mainImg =  basename($img); 
                                //echo $mainImg = end(explode('/', $img));
                ?>
                <div class="downloadInput" id="downloadLink">
                    <div class="noJsbtnDownRel">
                        <div class="noJsbtnDownAbs">
                            <input type="submit" class="noJsDownBtn" name="submit" value="Download Image">
                        </div>
                    </div>
                    <input type="hidden" name="image_name" value="<?php echo $mainImg; ?>">
                </div> 
            </div>

            <div class="clearfix"></div>
            <div class="gallery_inner">
                <?php
                if (!empty($photoListArray[$photoWCAGcounter]->photo_title)) {
                    ?>
                    <h3><?php echo t('Title'); ?>: <span class="phototitle"><?php echo stripslashes($photoListArray[$photoWCAGcounter]->photo_title); ?></span></h3>
                    <?php
                }
                if (!empty($photoListArray[$photoWCAGcounter]->photo_description)) {
                    ?>
                    <h3><?php echo t('Description'); ?>: <span class="photodesc"><?php echo stripslashes($photoListArray[$photoWCAGcounter]->photo_description); ?></span></h3>
                    <?php
                }

                if (!empty($photoListArray[$photoWCAGcounter]->photo_place)) {
                    ?>
                    <h3><?php echo t('Place'); ?>: <span class="photoplace"><?php echo stripslashes($photoListArray[$photoWCAGcounter]->photo_place); ?></span></h3> 
                    <?php
                }

                if (!empty($photoListArray[$photoWCAGcounter]->photo_people)) {
                    ?>
                    <h3><?php echo t('People'); ?>: <span class="photopeople"><?php echo stripslashes($photoListArray[$photoWCAGcounter]->photo_people); ?></span></h3>
                    <?php
                }
                if (!empty($photoListArray[$photoWCAGcounter]->photo_source)) {
                    ?>
                    <h3><?php echo t('Source'); ?>: <span class="photosource"><?php echo stripslashes($photoListArray[$photoWCAGcounter]->photo_source); ?></span></h3>                     <?php
                }
               
                ?> 
            </div>

            <div class="wcagPrevButton">
                <input type="submit" value=" " name="prevbutton" id="<?php echo (($photoWCAGcounter == 0) ? 'prevbutton_disable' : 'prevbutton') ?>" title="<?php echo t('Previous'); ?>" <?php echo (($photoWCAGcounter == 0) ? 'disabled="disabled"' : '') ?> />
            </div>
            <div class="wcagNextButton">
                <input type="submit" value=" " name="nextbutton" id="<?php echo ((($totalPhotoCount == ($photoWCAGcounter + 1))) ? 'nextbutton_disable' : 'nextbutton') ?>" title="<?php echo t('Next'); ?>" <?php echo ((($totalPhotoCount == ($photoWCAGcounter + 1))) ? 'disabled="disabled"' : '') ?> />        
            </div>
            <!--</form>-->
        </div>
		<noscript></noscript>
        <script type='text/javascript'>jQuery(document).ready(function ($) {
                $('#withoutjs').hide();
                $('#withjs').show();
            });</script>

        <?php $currentphoto = intval($photoWCAGcounter) + 1; ?>
        <label id="withoutjs" style="display:block; float:left; padding-left:10px;" ><?php echo t('Showing') . ": <span id='currentcount1'>" . $currentphoto . "</span> of " . $totalPhotoCount . " " . t('Photos'); ?></label>
    <?php } else { ?>
        <div class="galleryCon" style="min-height: 500px; text-align: center; font-weight: bold;"><?php echo t('No Record Found.') ?></div>
    <?php } ?>
    <div style="display: none;">
        <div id="printcontent" style="display:none;"></div>
        <div id="printPhotosId" class="printpopup">
            <div class="printphoto">
                <table id="print_radio" cellspacing="0" cellpadding="0" width="100%" >
                    <tr>
                        <td colspan="4" align="left" class="printpopupTitle"><?php echo t('Print Photo'); ?></td>
                    </tr>
                    <tr>
                        <td colspan="4" align="left"></td>
                    </tr>
                    <tr>
                        <td style="width:10%"  ><input type="radio" name="print_option" id="print_option_small" value="4x6" checked="checked" /></td>
                        <td style="width:90%"  colspan="3"><label for="print_option_small"> 4x6 </label></td> 
                    </tr>
                    <tr>
                        <td><input type="radio" name="print_option" id="print_option_medium" value="5x7" /></td>
                        <td colspan="3"><label for="print_option_medium"> 5x7</label> </td> 
                    </tr>
                    <tr>
                        <td><input type="radio" name="print_option" id="print_option_large" value="8x11" /></td>
                        <td colspan="3"><label for="print_option_large"> 8x11</label> </td> 
                    </tr>            
                    <tr>
                        <td colspan="4" style="height:10" class="none" >&nbsp;</td>
                    </tr>
                    <tr>
                        <td colspan="4" align="right" class="print_popup_buttons">
                            <div class="print_popup_buttons">
                                <input type="button" name="btnPrint" id="btnPrint" onclick="print_photos('<?php echo $base_url ?>');" class="searchsubmit" value="<?php echo t('Print') ?>" />   
                                <input type="button" class="searchsubmit" name="printCancel" id="printCancel" value="<?php echo t('Cancel') ?>" />
                            </div>
                        </td>
                    </tr>
                </table> 
            </div>
        </div>
    </div>
    <div class="copyrightBottom">
        <div class="copyBotimg">
            <img src="<?php echo $base_url; ?>/sites/all/themes/ghp/images/copyright-info.jpg" title = "copyright" alt = "copyright"/>
        </div>
        <div id="copyright_content">
        <?php
      if(isset($photoid) && $photoid != ''){
        $copyright_detail = "SELECT pm.photo_id, c.copyright_id, c.title, c.description FROM copyright c INNER JOIN photo_master pm ON c.copyright_id = pm.copyright_id where pm.photo_id = " . $photoid . " group by pm.photo_id";
        $copyrights = db_query($copyright_detail)->fetchAll();
        }
        if (!empty($copyrights)) {
            $title = $copyrights1[0]->title;
            $description = $copyrights[0]->description;
            echo $description;
        } else {
        
            $copyright_list = "SELECT cm.copyright_id, c.title, c.description FROM copyright c INNER JOIN copyright_module cm ON c.copyright_id=cm.copyright_id WHERE module_name = 'photos'";
            $copyrights = db_query($copyright_list)->fetchAll();

            if (!empty($copyrights)) {
		       $description = $copyrights[0]->description;
        	   echo $description;
            } else {
                $copyright_default = "SELECT c.title, c.description FROM copyright c WHERE c.default_flag='1'";
                $copyrights = db_query($copyright_default)->fetchAll();
                $description = $copyrights[0]->description;
                echo $description;
            }
        } 
        ?>
        </div> 
				
    </div>	

    <!--Start Code For Other References -->		
    <?php

    $video_url = variable_get('ghp_videos_path');

    $video_player_image_url = $video_url . 'images/';
    $get_cartoon_path = variable_get('ghp_cartoons_path');
    $cartoon_medium_url = $get_cartoon_path . 'medium/';
    $poster_medium_path = variable_get('ghp_posters_path') . "medium/";
    $poster_main_image_path = variable_get('ghp_posters_path') . "main_image/";
    $videoModuleUrl = url(drupal_get_path('module', 'ghp_videos'), array('absolute' => true)) . '/';
    $stamp_url = variable_get('ghp_stamps_path');
    $get_audio_path = variable_get('ghp_audios_path');
    $tribute_url = variable_get('ghp_tribute_path');

    $videoCount = (isset($videoCount) && $videoCount != '') ? $videoCount : '0';
    $cartoonCount = (isset($cartoonCount) && $cartoonCount != '') ? $cartoonCount : '0';
    $audioCount = (isset($audioCount) && $audioCount != '') ? $audioCount : '0';
    $posterCount = (isset($posterCount) && $posterCount != '') ? $posterCount : '0';
    $stampCount = (isset($stampCount) && $stampCount != '') ? $stampCount : '0';
    $cwmgCount = (isset($cwmgCount) && $cwmgCount != '') ? $cwmgCount : '0';
    $tributeCount = (isset($tributeCount) && $tributeCount != '') ? $tributeCount : '0';
// condition for the other ref
  	$ghp_otherreference_module_setting = explode(",",variable_get('ghp_otherreference_module_setting'));
  	$val_otherref = in_array('PH',$ghp_otherreference_module_setting);	
  	if(!empty($val_otherref)){
	?>
	<!-- JS enable code -->
    <div class="manuscriptTabs" id="referenceItem" style="" class="ui-widget-content ui-corner-all">
        <div class="manuscriptTabstop">
            <a class="otherRef" style="" title="<?php echo t('Other References'); ?>" href="javascript:;"> 
                <h2>
                    <?php echo t('Other References'); ?>
                    <img style="vertical-align:middle;" id="arrowimage" src="<?php echo $base_url; ?>/sites/default/files/ghp_photos/images/downarrow.png" />
                </h2>    
            </a>    
        </div>

        <div class="referenceContent" id="with_js" style="display:none;">
            <div class="referenceContentTop"></div>
            <div class="referenceContentMid clearfix">
                <div class="data_head"></div>
                <div class="data_detail"></div>
                <div class="ajax_loader" id="ajax_loader" style='display:none;'>
                    <img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" />
                </div> 
                <div class="btnHolder"></div>
            </div>
            <div class="referenceContentBot"></div>

        </div>
        
        <div class="referenceContent_1" id="without_js" >
            <div class="referenceContentTop"></div>
            <div class="referenceContentMid clearfix">
               <?php
               if($other_ref){
                   print_r($other_ref);
               }
               ?>
                
                <div class="ajax_loader" id="ajax_loader" style='display:none;'>
                    <img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" />
                </div> 
                <div class="btnHolder"></div>
            </div>
            <div class="referenceContentBot"></div>

        </div>
        
        
    </div>
    <?php } ?>
    
     
     
    <!-- JS disable code -->
    
    <input type="hidden" name="jsstatus" id="jsstatus" value="0" readonly="readonly" />
    <input type="hidden" name="activeTab" id="activeTab" value="video" readonly="readonly" />
    <input type="hidden" name="photo_id" id="photo_id" value="<?php echo base64_decode(arg(1)); ?>" readonly="readonly" />
    <input type="hidden" name="base_url" id="base_url" value="<?php echo $base_url; ?>" readonly="readonly" />
    <input type="hidden" name="trans_url" id="trans_url" value="<?php echo $trans_url; ?>" readonly="readonly" />
    <input type="hidden" name="photo_path" id="photo_path" value="<?php echo variable_get('ghp_photos_path'); ?>" readonly="readonly" />
    <input type="hidden" name="cartoon_path" id="cartoon_path" value="<?php echo $cartoon_medium_url; ?>" readonly="readonly" />
    <input type="hidden" name="poster_path" id="poster_path" value="<?php echo $poster_medium_path; ?>" readonly="readonly" />
    <input type="hidden" name="poster_path_main" id="poster_path_main" value="<?php echo $poster_main_image_path; ?>" readonly="readonly" />
    <input type="hidden" name="stamp_path" id="stamp_path" value="<?php echo $stamp_url; ?>" readonly="readonly" />
    <input type="hidden" name="tribute_path" id="tribute_path" value="<?php echo $tribute_url; ?>" readonly="readonly" />
    <input type="hidden" name="audio_url" id="audio_url" value="<?php echo $get_audio_path; ?>" readonly="readonly" />
    <input type="hidden" name="tribute_url" id="tribute_url" value="<?php echo $tribute_url; ?>" readonly="readonly" />
    
      <input type="hidden" name="language" id="language" value="<?php echo $language->language; ?>" readonly="readonly" />
    <!--End Code For Other References -->			
</form>
<script type="text/javascript">
jQuery(document).ready(function($){
    hs.graphicsDir = '<?php echo $graphicsFolderUrl; ?>';
	hs.align = 'center';
	hs.transitions = ['expand', 'crossfade'];
	hs.outlineType = 'rounded-white';
	hs.fadeInOut = true;
	hs.dimmingOpacity = 0.75;

	// define the restraining box
	hs.useBox = false;
	hs.width = 640;
	hs.height = 480;

	// Add the controlbar
	hs.addSlideshow({
		//slideshowGroup: 'group1',
		interval: 5000,
		repeat: false,
		useControls: true,
		fixedControls: 'fit',
		overlayOptions: {
			opacity: 1,
			position: 'top center',
			hideOnMouseOut: true
		}
	});

});
</script>