<?php
global $base_path;
global $base_url;
global $language;
drupal_add_css(drupal_get_path('module', 'ghp_tributes') . '/css/tributestogandhiji.css');
$ghp_data_root_path = variable_get('ghp_data_root_path');
$templateImgPath = $base_url . '/sites/all/themes/ghp/images/';
$rootImgPath = $base_url . '/sites/default/files/';
$photoModuleToken = md5(microtime());
$_SESSION['photoModuleToken'] = $photoModuleToken;
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

    $defaultlan = language_default();
    $trans_url = $base_url;
    if($language->language != $defaultlan->language) { 
        $trans_url .= "/".$language->language;
    }

    $photoid = filtertext(strip_tags(base64_decode(arg(1))));
    $ghp_photos_path = variable_get('ghp_photos_path');

    $resultimg = db_query("SELECT pm.photo_id,pm.photo_filename,pd.photo_title, pd.photo_description, pd.photo_place, pd.photo_source, pd.photo_people, pd.photographer_name FROM photo_master as pm inner join photo_detail as pd on(pd.photo_id = pm.photo_id) WHERE pm.photo_id='" . $photoid . "' AND pd.language_code='" . $language->language . "'");
    $photo = $resultimg->fetchObject();
?>
	 <div class="cartoondeatlwcag">
            <a class="backbtn" href="<?php echo $trans_url; ?>/photos_thumbview" title="<?php echo t('Back');?>"> <?php echo 
			t('Back');?></a> <?php /*<a id="rssvolume" target="_blank" href="<?php echo $trans_url; ?>/photos/rss/<?php echo base64_encode($photo->photo_id); ?>" style="margin: 0px 10px 10px 0px;" class="rssfeed" title="Rss"></a> */ ?></div>
            <div class="cartoneImg">
                <img src="<?php echo $ghp_photos_path . $photo->photo_filename; ?>" alt=""  />
     </div>
  
     <div class="gallery_inner">
	   
   <?php
   if(!empty($photo->photo_title))
   { ?>                    
        <h3><?php echo t('Title');?> : <span class="phototitle"> <?php echo stripslashes($photo->photo_title);?> </span></h3>
  <?php 
  }
  
   if(!empty($photo->photo_description))
   { ?>                    
		<h3><?php echo t('Description');?> : <span class="phototitle"> <?php echo stripslashes($photo->photo_description);?> </span></h3>
   <?php
   }   
   if(!empty($photo->photo_place))
   {   ?>                  
        <h3> <?php echo t('Place'); ?> : <span class="phototitle"><?php echo stripslashes($photo->photo_place); ?> </span></h3>
   <?php
   }
   if(!empty($photo->photo_people))
   { ?>                   
      <h3> <?php echo t('People');?> : <span class="phototitle"><?php echo stripslashes($photo->photo_people);?> </span></h3>
   <?php
   }
   if(!empty($photo->photo_source))
   {  
   ?>                   
       <h3><?php echo t('Source'); ?> : <span class="phototitle"><?php echo stripslashes($photo->photo_source); ?></span></h3>
  <?php 	} ?>
	</div>
 
<div class="copyrightBottom">
	 <div class="copyBotimg">
			<img src="<?php echo $base_url;?>/sites/all/themes/ghp/images/copyright-info.jpg" />
		</div>
		<?php
			//Copyright at detail(item) level					
			$copyright_detail = "SELECT pm.photo_id, c.copyright_id, c.title, c.description FROM copyright c INNER JOIN photo_master pm ON c.copyright_id = pm.copyright_id where pm.photo_id = ".$photoid." group by pm.photo_id";	
			$copyrights = db_query($copyright_detail)->fetchAll();
			//echo "<pre>"; 
			//print_r($copyrights); exit;
			if(!empty($copyrights))
			{
				//print_r($copyrights); exit;
				$title = $copyrights[0]->title;
				$description = $copyrights[0]->description;
				echo $description;
			
			}
			else
			{
				//Copyright at list(module) level
				$copyright_list = "SELECT cm.copyright_id, c.title, c.description FROM copyright c INNER JOIN copyright_module cm ON c.copyright_id=cm.copyright_id WHERE module_name = 'photos'";
				$copyrights = db_query($copyright_list)->fetchAll();
				
				if(!empty($copyrights))
				{	
					//print_r($copyrights); exit;
					$title = $copyrights[0]->title;
					$description = $copyrights[0]->description;
					echo $description;
				}
				else
				{
					//Copyright from by default flag
					$copyright_default = "SELECT c.title, c.description FROM copyright c WHERE c.default_flag='1'";
					$copyrights = db_query($copyright_default)->fetchAll();
					
					//print_r($copyrights); exit;
					$title = $copyrights[0]->title;
					$description = $copyrights[0]->description;
					echo $description;
				}
			}
			
		?>
</div> 
 
<?php
require_once('Mobile_Detect.php');
    $detect = new Mobile_Detect();
    $detect_mobile = $detect->isMobile();
    if ($detect_mobile === true) {
        $mobile_path = "240";
        $mobileclass = "active";
        $pcclass = "";
    } else {
        $mobile_path = "360";
        $mobileclass = "";
        $pcclass = "active";
    }
	
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
	
    // tab array, based on new reference items add module name here first !!!
    $tabArray = array('video', 'cartoon', 'audio', 'poster', 'stamp', 'cwmg', 'tribute');
    $activeTab = arg(2);
    if (!in_array($activeTab, $tabArray)) {
        $activeTab = 'video';
    }
	//echo $activeTab;
	//exit;
    ?>
	
    <div class="manuscriptTabs" id="referenceItem">
          <div class="manuscriptTabstop">
              <h2><?php echo t('Other References'); ?></h2>
          </div>
		  
          <div class="referenceContent">
              <div class="referenceContentTop"></div>
              <div class="referenceContentMid clearfix">
                  <ul class="refrenceTab">
                      <li><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/video/#referenceItem" <?php if ($activeTab == 'video') { ?> class="active" <?php } ?> ><?php echo t('Videos'); ?> (<?php echo $videoCount; ?>)</a></li>
                      <li><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/cartoon" <?php if ($activeTab == 'cartoon') { ?> class="active" <?php } ?>><?php echo t('Cartoons'); ?> (<?php echo $cartoonCount; ?>)</a></li>
                                          <li><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/audio" <?php if ($activeTab == 'audio') { ?> class="active" <?php } ?>><?php echo t('Audios'); ?> (<?php echo $audioCount; ?>)</a></li>
                                          <li><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/poster" <?php if ($activeTab == 'poster') { ?> class="active" <?php } ?>><?php echo t('Posters'); ?> (<?php echo $posterCount; ?>)</a></li>
                      <li><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/stamp" <?php if ($activeTab == 'stamp') { ?> class="active" <?php } ?> ><?php echo t('Stamps'); ?> (<?php echo $stampCount; ?>)</a></li>
                      <li><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/cwmg" <?php if ($activeTab == 'cwmg') { ?> class="active" <?php } ?> ><?php echo t('CWMG'); ?> (<?php echo $cwmgCount; ?>)</a></li>
                                          <li class="last"><a href="<?php echo $base_url; ?>/photosdetail/<?php echo arg(1); ?>/tribute" <?php if ($activeTab == 'tribute') { ?> class="active" <?php } ?> ><?php echo t('Tributes'); ?> (<?php echo $tributeCount; ?>)</a></li>
                  </ul>
                  <input type="hidden" name="phototitle" id="phototitle" value="<?php echo t('Title'); ?>" />
                  <?php if ($activeTab == 'video') { ?>
                      <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $videoCount; ?> <?php echo t('results found'); ?> </div>
                <?php } ?>
                <?php if ($activeTab == 'cartoon') { ?>
                    <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $cartoonCount; ?> <?php echo t('results found'); ?></div>
                <?php } ?>
                <?php if ($activeTab == 'audio') { ?>
                    <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $audioCount; ?> <?php echo t('results found'); ?></div>
                <?php } ?>
                <?php if ($activeTab == 'poster') { ?>
                    <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $posterCount; ?> <?php echo t('results found'); ?></div>
                <?php } ?>
                <?php if ($activeTab == 'stamp') { ?>
                    <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $stampCount; ?> <?php echo t('results found'); ?></div>
                <?php } ?>
                <?php if ($activeTab == 'cwmg') { ?>
                    <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $cwmgCount; ?> <?php echo t('results found'); ?></div>
                <?php } ?>    
				<?php if ($activeTab == 'tribute') { ?>
                    <div class="refrenceResult"><?php echo t('About'); ?> <?php echo $tributeCount; ?> <?php echo t('results found'); ?></div>
                <?php } ?>
               
                <!--Video listing-->
                <?php if ($activeTab == 'video') { ?>
                    <ul class="referenceList wcagVideoList " id="videoGallery">
                        <?php
                        if (count($photoReferencesVideo) > 0) {
                            foreach ($photoReferencesVideo as $keyVideo => $videos) {
                                $download = $base_url . "/ghpVideoDownload/" . $videos->video_id;
                                $videoFilesize = @filesize($ghp_data_root_path . 'ghp_videos/360/' . $videos->video_filename);
                                $videoFilesize = round($videoFilesize / 1048576, 2);
                                ?>
                                <li>
                                    <div class="referenceImg">
                                        <img src="<?php echo $video_url . 'images/medium/' . $videos->video_image; ?>" width="233" height="173" alt="<?php echo $videos->video_title; ?>" />
                                    </div>
                                    <div class="referenceContent">
                                        <h3><?php echo $videos->video_title; ?></h3>
                                        <h4><?php echo $videos->video_duration; ?></h4>
                                        <p><?php echo $videos->video_description; ?></p>
                                        <span class="spandownloadvideo"><a href="<?php echo $download ?>" class="title" title="<?php echo t('Download Video') ?>"><?php echo t('Download Video') ?></a></span>
                                        <div class="vsize"><?php echo t('Video Size') ?> : <?php echo $videoFilesize; ?> <?php echo t('Mb'); ?> </div>
                                    </div>
                                </li> 
                                <?php
                            }
                        }
                        ?>                        
                        <?php echo theme('pager', array('element' => 3)); ?>     
                    </ul>
                    <ul class="referenceList VideoList" id="videoGallery" style="display: none;">
                        <?php
                        if (count($photoReferencesVideo) > 0) {
                            foreach ($photoReferencesVideo as $keyVideo => $videos) {
                                ?>
                                <li>
                                    <div class="referenceImg">
                                        <a rel="gallery_video_group" id="showVideos_<?php echo $keyVideo; ?>" href="#showVideosId" class="productImage pop" title="<?php echo stripslashes($videos->video_title); ?>"> <img src="<?php echo $video_url . 'images/medium/' . $videos->video_image; ?>" width="233" height="173" alt="<?php echo $videos->video_title; ?>" /></a>
                                    </div>
                                    <div class="referenceContent">
                                        <h3><?php echo $videos->video_title; ?></h3>
                                        <h4><?php echo $videos->video_duration; ?></h4>
                                        <p><?php echo $videos->video_description; ?></p>                                                                                        
                                    </div>

                                    <input type="hidden" name="videoTitle<?php echo $keyVideo ?>" id="videoTitle<?php echo $keyVideo ?>" value="<?php echo $videos->video_title ?>" readonly="readonly" />
                                    <input type="hidden" name="videoDesc<?php echo $keyVideo ?>" id="videoDesc<?php echo $keyVideo ?>" value="<?php echo $videos->video_description ?>" readonly="readonly" />          
                                    <input type="hidden" name="videographerName<?php echo $keyVideo ?>" id="videographerName<?php echo $keyVideo ?>" value="<?php echo $videos->videographer_name ?>" readonly="readonly" />
                                    <input type="hidden" name="videoFilename<?php echo $keyVideo ?>" id="videoFilename<?php echo $keyVideo ?>" value="<?php echo $videos->video_filename ?>" readonly="readonly" />
                                    <input type="hidden" name="videoImage<?php echo $keyVideo ?>" id="videoImage<?php echo $keyVideo ?>" value="<?php echo $videos->video_image ?>" readonly="readonly" />                                
                                    <input type="hidden" name="videoImage_<?php echo $keyVideo ?>" id="videoImage_<?php echo $keyVideo ?>" value="<?php echo $videos->video_image; ?>" />

                                </li> 
                                <?php
                            }
                        }
                        ?>                        
                        <input type="hidden" name="videoModuleUrl" id="videoModuleUrl" value="<?php echo $videoModuleUrl ?>" readonly="readonly" />        
                    </ul>

                    <div style="display: none;">
                        <div id="showVideosId" class="printpopup">
                            <table id="print_radio" class="popupvideotable" border="0" cellspacing="2" cellpadding="2" width="440">
                                <tr>
                                    <td colspan="4" class="none" >
                                        <div id="jp_container_1" class="jp-video jp-video-270p">
                                            <div class="jp-type-playlist">
                                                <div id="jquery_jplayer_1" class="jp-jplayer"></div>
                                                <div class="jp-gui">
                                                    <div class="jp-video-play">
                                                        <a href="javascript:;" class="jp-video-play-icon" id="jp-video-play-icon" tabindex="1">play</a>
                                                    </div>
                                                    <div class="jp-interface" id="jp-interface">
                                                        <div class="jp-progress">
                                                            <div class="jp-seek-bar">
                                                                <div class="jp-play-bar"></div>
                                                            </div>
                                                        </div>
                                                        <div class="jp-current-time"></div>
                                                        <div class="jp-duration"></div>
                                                        <div class="jp-controls-holder" id="jp-controls-holder">
                                                            <ul class="jp-controls" id="jp-controls">
                                                                <li><a href="javascript:;" class="jp-previous" id="thumbPrev" tabindex="1" title="<?php echo t('Previous') ?>"><?php echo t('Previous') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-play" tabindex="1" title="<?php echo t('Play') ?>"><?php echo t('Play') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-pause" tabindex="1" title="<?php echo t('Pause') ?>"><?php echo t('Pause') ?></a></li>
                                                                <li><a href="javascript:;" onclick="nextPlayer();" class="jp-next" id="thumbNext" tabindex="1" title="<?php echo t('Next') ?>"><?php echo t('Next') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-stop" tabindex="1" title="<?php echo t('Stop') ?>"><?php echo t('Stop') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-mute" tabindex="1" title="<?php echo t('Mute') ?>"><?php echo t('Mute') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-unmute" tabindex="1" title="<?php echo t('Unmute') ?>"><?php echo t('Unmute') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-volume-max" tabindex="1" title="<?php echo t('Max Volume') ?>"><?php echo t('Max Volume') ?></a></li>
                                                            </ul>
                                                            <div class="jp-volume-bar">
                                                                <div class="jp-volume-bar-value"></div>
                                                            </div>
                                                            <ul class="jp-toggles">
                                                                <li><a href="javascript:;" class="jp-full-screen" tabindex="1" title="<?php echo t('Full Screen') ?>"><?php echo t('Full Screen') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-restore-screen" tabindex="1" title="<?php echo t('Restore Screen') ?>"><?php echo t('Restore Screen') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-shuffle" tabindex="1" title="<?php echo t('Shuffle') ?>"><?php echo t('Shuffle') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-shuffle-off" tabindex="1" title="<?php echo t('Shuffle Off') ?>"><?php echo t('Shuffle Off') ?></a></li> 
                                                                <li><a href="javascript:;" class="jp-repeat" tabindex="1" title="<?php echo t('Repeat') ?>"><?php echo t('Repeat') ?></a></li>
                                                                <li><a href="javascript:;" class="jp-repeat-off" tabindex="1" title="<?php echo t('Repeat Off') ?>"><?php echo t('Repeat Off') ?></a></li>
                                                            </ul>
                                                            <ul id="navbar">
                                                                <li><a href="javascript:;" class="jp-settings" tabindex="1"><span style="display: none;">Play Setting</span></a>
                                                                    <ul>
                                                                        <li style="text-align: left;"><span><b><?php echo t('Quality') ?></b></span></li>
                                                                        <li><a href="javascript:;" id="quality_720" ><?php echo t('High') ?></a></li>
                                                                        <li><a href="javascript:;" id="quality_360" class="<?php echo $pcclass ?>"><?php echo t('Medium') ?></a></li>
                                                                        <li><a href="javascript:;" id="quality_240" class="<?php echo $mobileclass ?>"><?php echo t('Average') ?></a></li>
                                                                    </ul>
                                                                </li>
                                                            </ul>
                                                        </div>
                                                        <div class="jp-title">
                                                            <ul>
                                                                <li></li>
                                                            </ul>
                                                        </div>
                                                    </div>
                                                </div>
                                                <div class="jp-playlist">
                                                    <ul>
                                                        <li></li>
                                                    </ul>
                                                </div>
                                                <div class="jp-no-solution">
                                                    <span>Update Required</span>
                                                    To play the media you will need to either update your browser to a recent version or update your <a href="http://get.adobe.com/flashplayer/" target="_blank">Flash plugin</a>.
                                                </div>
                                            </div>
                                        </div>              
                                    </td>
                                </tr>
                                <tr>
                                    <td colspan="4" align="left" class="videopopTitle" id="playVideoTitle"></td>
                                </tr>
                                <tr>
                                    <td colspan="4" align="left" class="videopopContent" id="playVideoDesc"></td>
                                </tr>            
                                <tr>
                                    <td colspan="4" class="videographyText" align="left" id="playVideographer"></td>
                                </tr>                        
                            </table> 
                            <input type="hidden" name="vtitle" id="vtitle" value="<?php echo t('Title'); ?>" />
                            <input type="hidden" name="vdesc" id="vdesc" value="<?php echo t('Description'); ?>" />
                            <input type="hidden" name="vvideographer" id="vvideographer" value="<?php echo t('Videographer'); ?>" /> 
                        </div>
                    </div>
                    <input type="hidden" value="video" name="activeTab" id="activeTab" >
                    <input type="hidden" value="<?php echo @$videos->video_id; ?>" name="videoId" id="videoId" autocomplete="off" >
                    <input type="hidden" value="<?php echo $videoCount; ?>" name="totalCount" id="totalCount" autocomplete="off" >
                    <input type="hidden" value="<?php echo count($photoReferencesVideo); ?>" name="displayCount" id="displayCount" autocomplete="off" >        
                    <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    <input type="hidden" name="video_url" id="video_url" value="<?php echo $video_url ?>" readonly="readonly" />
                    <input type="hidden" name="video_image_url" id="video_image_url" value="<?php echo $video_player_image_url ?>" readonly="readonly" />
                    <?php } ?>
                <!-- Cartoon listing-->     
                <?php if ($activeTab == 'cartoon') {
                    ?>    
                    <ul class="referenceList wcagPhotoList" >
                        <?php
                        if (count($photoReferencesCartoon) > 0) {
                            foreach ($photoReferencesCartoon as $cartoons) {
                                ?>
                                <li>
                                    <div class="referenceImg"><a href="<?php echo $base_url; ?>/cartoondetail/<?php echo base64_encode($cartoons->cartoon_id); ?>"> <img src="<?php echo $cartoon_medium_url . $cartoons->cartoon_filename; ?>" width="233" height="173" alt="<?php echo $cartoons->cartoon_title; ?>" /></a></div>
                                    <div class="referenceContent">
                                        <h3><?php echo $cartoons->cartoon_title; ?></h3>                                        
                                        <p><?php echo $cartoons->cartoon_description; ?></p>
                                        <div class="refViewmore"><a href="<?php echo $base_url; ?>/cartoondetail/<?php echo base64_encode($cartoons->cartoon_id); ?>"><?php echo t('View More'); ?></a></div>
                                    </div>
                                </li> 
                                <?php
                            }
                            echo '<input type="hidden" value="cartoon" name="activeTab" id="activeTab" >';
                        }
                        ?>
                        <?php echo theme('pager', array('element' => 4)); ?>                          
                    </ul>
                    <ul class="referenceList PhotoList" style="display:none;" >
                        <?php
                        if (count($photoReferencesCartoon) > 0) {
                            foreach ($photoReferencesCartoon as $key => $cartoons) {
                                ?>
                                <li>
                                    <div class="referenceImg"><a rel="gallery_cartoon_group" href="<?php echo $cartoon_medium_url . $cartoons->cartoon_filename; ?>" class="productImage pop" title="<?php echo stripslashes($cartoons->cartoon_title); ?>"> <img src="<?php echo $cartoon_medium_url . $cartoons->cartoon_filename; ?>" width="233" height="173" alt="<?php echo $cartoons->cartoon_title; ?>" /></a></div>                                    
                                    <div class="referenceContent">
                                        <h3><?php echo $cartoons->cartoon_title; ?></h3>                                        
                                        <p><?php echo $cartoons->cartoon_description; ?></p>
                                        <div class="refViewmore"><a href="<?php echo $base_url; ?>/cartoondetail/<?php echo base64_encode($cartoons->cartoon_id); ?>"><?php echo t('View More'); ?></a></div>
                                    </div>                                    
                                </li> 
                                <?php
                            }
                            echo '<input type="hidden" value="cartoon" name="activeTab" id="activeTab" >';
                            echo '<input type="hidden" value="' . $cartoons->cartoon_id . '" name="cartoonId" id="cartoonId" autocomplete="off" >';
                            echo '<input type="hidden" value="' . $cartoonCount . '" name="totalCount" id="totalCount" autocomplete="off" >';
                            echo '<input type="hidden" value="0" name="displayCount" id="displayCount" autocomplete="off" >';
                        }
                        ?>
					   <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    </ul>
                <?php } ?>  
				<!-- Audio listing-->     
                <?php
                if ($activeTab == 'audio') {
                    //echo '<pre>';
                   // print_r($photoReferencesAudio); exit;
                    ?>    
                    <ul class="referenceList wcagAudioList" id="wcagAudioList" >
                        <?php
                        if (count($photoReferencesAudio) > 0) {
								foreach ($photoReferencesAudio as $audios) {
                                ?>
                                <li>                                    
                                    <div class="referenceContent">
                                        <h3><?php echo $audios->audio_title; ?></h3>                                        
                                        <p><?php echo t("Date") ?>: <?php echo $audios->audio_date; ?></p>
                                        <span class="spandownloadaudio"><?php echo t(' Download Audio File: '); ?><a href="<?php echo $base_url ?>/ghpaudiodownload/<?php echo base64_encode($audios->audio_id); ?>" title='<?php echo $audios->audio_filename; ?>'><?php echo $audios->audio_filename ?></a></span>
                                    </div>
                                </li> 
                                <?php
                            }
                            echo '<input type="hidden" value="audio" name="activeTab" id="activeTab" >';
                        }
                        ?>
                        <?php echo theme('pager', array('element' => 5)); ?>                          
                    </ul>
                    <ul class="referenceList AudioList" id="AudioList" style="display:none;" >
                        <?php
                        if (count($photoReferencesAudio) > 0) {
                            $create_div = '';
                            foreach ($photoReferencesAudio as $keyAudio => $audios) {
                                if ($keyAudio != 0) {
                                    $style = "style='display:none;'";
                                } else {
                                    $style = '';
                                }
                                $create_div .= '<div class="jp-titleBar"><div class="audio_detail_' . $keyAudio . '"  ' . $style . '>';
                                $create_div .='<h3>' . $audios->audio_title . '</h3><div class="mainTitle"><span>' . t('Date') . ': ' . $audios->audio_date . '</span>' . t('Location') . ': ' . $audios->audio_place . '</div>';
                                $create_div .= '<div class="slideshow"><div class="imagehead">';
                                $create_div .= '<input type="hidden" class="volumerangeval"  name="volumerangeval"  value="' . $audios->actualvolid . "##" . $audios->volumepagerange . '" />';
                                $create_div .= '</div>';
                                $create_div .='<div id="imagecontainervol_' . $keyAudio . '"  class="imagecontainer" >';
                                $create_div .='<ul id="mycarousel_' . $keyAudio . '" class="jcarousel-skin-tango">';
                                if ($audios->actualvolid != '') {
                                    if (is_dir($ghp_data_root_path . 'volumes/' . $language->language . '/enhanced/vol' . $audios->actualvolid) == true) {
                                        $volume_flag = true;

                                        if (!empty($audios->volumepagerange)) {

                                            $create_div .='<input type="hidden" name="volumeFlag_' . $keyAudio . '" id="volumeFlag_' . $keyAudio . '" value="' . $volume_flag . '" />';
                                            $create_div .='<input type="hidden" name="volumedata_' . $keyAudio . '" id="volumedata_' . $keyAudio . '" value="' . $audioArray->actualvolid . "##" . $audioArray->volumepagerange . '" />';

                                            $page_range = $audios->volumepagerange;
                                            list($page_from, $page_to) = explode("-", $page_range);

                                            for ($i = $page_from; $i <= $page_to; $i++) {
                                                $f_name = "vol_" . $audios->actualvolid . "_" . $language->language . "_" . $i . ".jpg";



                                                $create_div .= '<li>
                                                        <img class="carouselimg"  src="' . $CWMG_VOLUME_PATH . $language->language . "/enhanced/vol" . $audioArray->actualvolid . "/" . $f_name . '" alt="" />
                                                    </li>';
                                            }
                                        }
                                    } else {
                                        $create_div .='<li>
                                                <img class="carouselimg"  src="' . $base_url . '/sites/all/themes/ghp/images/no_audio_text.jpg" alt="" />
                                            </li>';
                                    }
                                }

                                $create_div .='</ul>';
                                $create_div .='</div>';
                                $create_div .='</div>';

                                $create_div .='</div></div>';
                                
                                ?>
                                <li>                                    
                                    <div class="referenceContent">
                                        <h3>
                                            <a rel="gallery_audio_group" id="showAudios_<?php echo $keyAudio; ?>" href="#showAudioId" title="<?php echo stripslashes($audios->audio_title); ?>"><?php echo $audios->audio_title; ?></a>
                                            <input type="hidden" name="audioFile_<?php echo $keyAudio ?>" id="audioFile_<?php echo $keyAudio ?>" value="<?php echo $audios->audio_filename; ?>" />
                                        </h3>                                        
                                        <p><?php echo t("Date") ?>: <?php echo $audios->audio_date; ?></p>                                        
                                    </div>                                    
                                </li> 
            <?php
        }
        echo '<input type="hidden" value="audio" name="activeTab" id="activeTab" >';
        echo '<input type="hidden" value="' . $audios->audio_id . '" name="audioId" id="audioId" autocomplete="off" >';
        echo '<input type="hidden" value="' . $audioCount . '" name="totalCount" id="totalCount" autocomplete="off" >';
        echo '<input type="hidden" value="' . count($photoReferencesAudio) . '" name="displayCount" id="displayCount" autocomplete="off" readonly="readonly" >';
        echo '<input type="hidden" value="' . t("Date") . '" name="dateTitle" id="dateTitle" autocomplete="off" readonly="readonly" >';
    }
    ?>                        
                        <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    </ul>
                    <div style="display: none;">
                        <div class="audioPlayerWrapper none" id="showAudioId">
                            <div id="jquery_jplayer_2" class="jp-jplayer"></div>
                            <div id="jp_container_2" class="jp-audio">
                                <div class="jp-type-playlist">
                                    <div class="jp-gui jp-interface">
                                        <ul class="jp-controls">
                                            <li><a href="javascript:;" class="jp-previous" id="audioPrev" tabindex="1" title="<?php echo t('Previous') ?>"><?php echo t('Previous') ?></a></li>
                                            <li><a href="javascript:;" class="jp-play" tabindex="1" title="<?php echo t('Play') ?>"><?php echo t('Play') ?></a></li>
                                            <li><a href="javascript:;" class="jp-pause" tabindex="1" title="<?php echo t('Pause') ?>"><?php echo t('Pause') ?></a></li>
                                            <li><a href="javascript:;" class="jp-next" id="audioNext" tabindex="1" title="<?php echo t("Next") ?>"><?php echo t("Next") ?></a></li>
                                            <li><a href="javascript:;" class="jp-stop" tabindex="1" title="<?php echo t("Stop") ?>"><?php echo t("Stop") ?></a></li>
                                            <li><a href="javascript:;" class="jp-mute" tabindex="1" title="<?php echo t("Mute") ?>"><?php echo t("Mute") ?></a></li>
                                            <li><a href="javascript:;" class="jp-unmute" tabindex="1" title="<?php echo t("Unmute") ?>"><?php echo t("Unmute") ?></a></li>
                                            <li><a href="javascript:;" class="jp-volume-max" tabindex="1" title="<?php echo t("Max Volume") ?>"><?php echo t("Max Volume") ?></a></li>
                                        </ul>
                                        <div class="jp-progress">
                                            <div class="jp-seek-bar">
                                                <div class="jp-play-bar"></div>
                                            </div>
                                        </div>
                                        <div class="jp-volume-bar">
                                            <div class="jp-volume-bar-value"></div>
                                        </div>
                                        <div class="jp-time-holder">
                                            <div class="jp-current-time"></div>
                                            <div class="jp-duration"></div>
                                        </div>
                                        <ul class="jp-toggles">
                                            <li><a href="javascript:;" class="jp-shuffle" tabindex="1" title="<?php echo t("Shuffle") ?>"><?php echo t("Shuffle") ?></a></li>
                                            <li><a href="javascript:;" class="jp-shuffle-off" tabindex="1" title="<?php echo t("Shuffle Off") ?>"><?php echo t("Shuffle Off") ?></a></li>
                                            <li><a href="javascript:;" class="jp-repeat" tabindex="1" title="<?php echo t("Repeat") ?>"><?php echo t("Repeat") ?></a></li>
                                            <li><a href="javascript:;" class="jp-repeat-off" tabindex="1" title="<?php echo t("Repeat Off") ?>"><?php echo t("Repeat Off") ?></a></li>
                                        </ul>
                                        <ul id="navbar">
                                            <li><a href="javascript:;" class="jp-settings" tabindex="1"><span style="display: none;"><?php echo t('jpSetting'); ?></span></a>
                                                <ul>
                                                    <li style="text-align: left;"><span><b><?php echo t("Quality") ?></b></span></li>
                                                    <li><a href="javascript:;" id="quality_720" ><?php echo t("High") ?></a></li>
                                                    <li><a href="javascript:;" id="quality_360" class="<?php echo $pcclass ?>"><?php echo t("Medium") ?></a></li>
                                                    <li><a href="javascript:;" id="quality_240" class="<?php echo $mobileclass ?>"><?php echo t("Average") ?></a></li>
                                                </ul>
                                            </li>
                                        </ul>
                                    </div>
                                    <div id="audio_detail">  
    <?php echo $create_div; ?>  
                                    </div>  
                                    <div class="jp-playlist">
                                        <ul>
                                            <li></li>
                                        </ul>
                                    </div>
                                    <div class="jp-no-solution"> <span>Update Required</span> To play the media you will need to either update your browser to a recent version or update your <a href="http://get.adobe.com/flashplayer/" target="_blank">Flash plugin</a>. </div>
                                </div>
                            </div>
                        </div>                        
                        <div class="noticefullscreen"><span><?php echo t("Important note") . ': ' ?></span><?php echo t("THE TEXT DISPLAYED WITH THIS AUDIO IS TAKEN FROM CWMG AND MAY NOT MATCH WORD BY WORD WITH THE SPEECH.") ?></div>       
                        <div class="lyricsBlock" style="display:none">
                            <div id="volumecontentdisplay" class="ad-gallery" style="float: left; clear: both;">
                                <div class="container">
                                    <div class="slideshow">
                                        <div id="volumeJs" class="head">
                                            <button class="fullscreen_btn requestfullscreen" id="fullscreen_display" title="View Fullscreen"></button>
                                            <button class="fullscreenicon exitfullscreen" id="fullscreen_display1" title="Exit Fullscreen" style="display:none"></button>
                                            <button title="<?php t('Print Volume Pages related to Audio'); ?>"  id="printvolume" class="printBtn"></button>                                                               
                                        </div>
                                        <div id="imagecontainervol">
                                            <ul id="mycarousel" class="jcarousel-skin-tango">
    <?php
    if ($photoReferencesAudio[0]->actualvolid != '') {
        if (is_dir($ghp_data_root_path . 'volumes/' . $language->language . '/enhanced/vol' . $photoReferencesAudio[0]->actualvolid) == true) {
            if (!empty($photoReferencesAudio[0]->VolumePageRange)) {
                $page_range = $photoReferencesAudio[0]->VolumePageRange;
                list($page_from, $page_to) = explode("-", $page_range);

                for ($i = $page_from; $i <= $page_to; $i++) {
                    $f_name = "vol_" . $photoReferencesAudio[0]->actualvolid . "_" . $language->language . "_" . $i . ".jpg";
                    ?>
                                                                <li>
                                                                    <img class="carouselimg"  src="<?php echo $CWMG_VOLUME_PATH . $language->language . "/enhanced/vol" . $photoReferencesAudio->actualvolid . "/" . $f_name; ?>" alt="" width="420" height="595" />
                                                                </li>
                    <?php
                }
            }
        } else {
            ?>
                                                        <li>
                                                            <img class="carouselimg"  src="<?php echo $base_url . '/sites/all/themes/ghp/images/no_audio_text.jpg' ?>" alt="" width="420" height="595" />
                                                        </li>

            <?php
        }
    }
    ?>
                                            </ul>
                                        </div>                               
                                    </div>
                                </div>
                            </div>			    
                        </div>
                    </div>
                    <input name="volumeclickid" id="volumeclickid" type="hidden"  value="0" />                    

<?php } ?>
					                    
                         <!-- Poster listing-->     
                <?php if ($activeTab == 'poster') { ?>    
                    <ul class="referenceList wcagPhotoList" >
                    <?php
                    if (count($photoReferencesPoster) > 0) {
                        foreach ($photoReferencesPoster as $poster) {
                            ?>
                                <li>
                                    <div class="referenceImg"><a href="<?php echo $base_url; ?>/postersdetail/<?php echo base64_encode($poster->poster_id); ?>"> <img src="<?php echo $poster_medium_path . $poster->poster_filename; ?>" width="233" height="173" alt="<?php echo $poster->poster_title; ?>" /></a></div>
                                    <div class="referenceContent">
                                        <h3><?php echo $poster->poster_title; ?></h3>                                        
                                        <p><?php echo $poster->poster_description; ?></p>
                                        <div class="refViewmore"><a href="<?php echo $base_url; ?>/postersdetail/<?php echo base64_encode($poster->poster_id); ?>"><?php echo t('View More'); ?></a></div>
                                    </div>
                                </li> 
            <?php
        }
        echo '<input type="hidden" value="poster" name="activeTab" id="activeTab" >';
    }
    ?>
                        <?php echo theme('pager', array('element' => 6)); ?>                          
                    </ul>
                    <ul class="referenceList PhotoList" style="display:none;" >
    <?php
    if (count($photoReferencesPoster) > 0) {
        foreach ($photoReferencesPoster as $poster) {
            ?>
                                <li>
                                    <div class="referenceImg"><a rel="example_group" href="<?php echo $poster_main_image_path . $poster->poster_filename; ?>" class="productImage pop" title="<?php echo stripslashes($poster->poster_title); ?>"> <img src="<?php echo $poster_medium_path . $poster->poster_filename; ?>" width="233" height="173" alt="<?php echo $poster->poster_title; ?>" /></a></div>
                                    <div class="referenceContent">
                                        <h3><?php echo $poster->poster_title; ?></h3>                                        
                                        <p><?php echo $poster->poster_description; ?></p>
                                        <div class="refViewmore"><a href="<?php echo $base_url; ?>/postersdetail/<?php echo base64_encode($poster->poster_id); ?>"><?php echo t('View More'); ?></a></div>
                                    </div>
                                </li> 
            <?php
        }
        echo '<input type="hidden" value="poster" name="activeTab" id="activeTab" >';
        echo '<input type="hidden" value="' . $poster->poster_id . '" name="posterId" id="posterId" autocomplete="off" >';
        echo '<input type="hidden" value="' . $posterCount . '" name="totalCount" id="totalCount" autocomplete="off" >';
        echo '<input type="hidden" value="0" name="displayCount" id="displayCount" autocomplete="off" >';
    }
    ?>

                        <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    </ul>

<?php } ?>
                <!-- Stamp listing-->     
                <?php if ($activeTab == 'stamp') { ?>    
                    <ul class="referenceList wcagPhotoList" >
                    <?php
                    if (count($photoReferencesStamp) > 0) {
                        foreach ($photoReferencesStamp as $stamp) {
                            ?>
                                <li>
								   <div class="referenceImg"><a href="<?php echo $base_url; ?>/stampsdetail/<?php echo base64_encode($stamp->stamp_id); ?>"><img src="<?php echo $stamp_url . $stamp->stamp_filename; ?>" width="233" height="173" alt="<?php echo $stamp->stamp_title; ?>" /></div>
                                   <div class="referenceContent">
                                        <h3><?php echo $stamp->stamp_title; ?></h3>                                        
                                        <p><?php echo $stamp->slug; ?></p>
                                        <div class="refViewmore"><a href="<?php echo $base_url; ?>/stampsdetail/<?php echo base64_encode($stamp->stamp_id); ?>"><?php echo t('View More'); ?></a></div>
                                    </div>
                                </li> 
            <?php
        }
        echo '<input type="hidden" value="stamp" name="activeTab" id="activeTab" >';
    }
    ?>
                        <?php echo theme('pager', array('element' => 7)); ?>                          
                    </ul>
                    <ul class="referenceList PhotoList" style="display:none;" >
    <?php
    if (count($photoReferencesStamp) > 0) {
        foreach ($photoReferencesStamp as $stamp) {
            ?>
                               <li>
                                    <div class="referenceImg"><a rel="example_group" href="<?php echo $stamp_url . $stamp->stamp_filename; ?>" class="productImage pop" title="<?php echo stripslashes($stamp->stamp_title); ?>"> <img src="<?php echo $stamp_url . $stamp->stamp_filename; ?>" width="233" height="173" alt="<?php echo $stamp->stamp_title; ?>" /></a></div>
                                    
									 <div class="referenceContent">
                                        <h3><?php echo $stamp->stamp_title; ?></h3>                                        
                                        <p><?php echo $stamp->slug; ?></p>
                                        <div class="refViewmore"><a href="<?php echo $base_url; ?>/stampsdetail/<?php echo base64_encode($stamp->stamp_id); ?>"><?php echo t('View More'); ?></a></div>
                                    </div>
                                </li> 
            <?php
        }
        echo '<input type="hidden" value="stamp" name="activeTab" id="activeTab" >';
        echo '<input type="hidden" value="' . $stamp->stamp_id . '" name="stampId" id="stampId" autocomplete="off" >';
        echo '<input type="hidden" value="' . $stampCount . '" name="totalCount" id="totalCount" autocomplete="off" >';
        echo '<input type="hidden" value="0" name="displayCount" id="displayCount" autocomplete="off" >';
    }
    ?>
                        <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    </ul>

<?php } ?>
                <!-- CWMG listing-->     
                <?php
                if ($activeTab == 'cwmg') {
				
                    ?>                        
                    <ul class="referenceList CwmgList" id="CwmgList"  >
                    <?php
                    if (count($photoReferencesCWMG) > 0) {
					    $cwmgWithItems = explode(';', $cwmgSections);
					    for ($i = 0; $i < count($cwmgWithItems); $i++) {
                            if ($cwmgWithItems[$i] != '')
                                $cwmgWithCategory[] = explode(':', $cwmgWithItems[$i]);
                        }
                        // seperate audio ids
                        for ($i = 0; $i < count($cwmgWithCategory); $i++) {
                            $cwmgWithIdArray[$cwmgWithCategory[$i][0]] = $cwmgWithCategory[$i][1];
                        }

                        foreach ($photoReferencesCWMG as $key => $cwmg) {
                            //$sectionKey = array_search($cwmg->actualvol,$cwmgWithIdArray);
                            ?>
                                <li>                                    
                                    <div class="referenceContent">
                                        <span class="cwmgListrefList"><?php echo $cwmg->volumename; ?> :</span>
            <?php
            if (isset($cwmgWithIdArray[$cwmg->actualvol])) {
                $volumePages = explode(',', $cwmgWithIdArray[$cwmg->actualvol]);
                $end = end($volumePages);

                foreach ($volumePages as $pages) {
                    $select = db_select('volumn_content_en', 'vce');
                    $select->fields('vce', array('PageNumber'));
                    $select->condition('ActualVolumeID', $cwmg->actualvol);
                    $select->condition('PageDisplayNumber', $pages);
                    $volumeDetail = $select->execute()->fetchAll();

                    if ($pages == $end)
                        echo '<a href="' . $base_url . '/cwmg_volume_thumbview/' . base64_encode($cwmg->actualvol) . '#page/' . $volumeDetail[0]->pagenumber . '/mode/2up">' . $pages . ' </a>';
                    else
                        echo '<a href="' . $base_url . '/cwmg_volume_thumbview/' . base64_encode($cwmg->actualvol) . '#page/' . $volumeDetail[0]->pagenumber . '/mode/2up">' . $pages . ' </a> , ';
                }
            }
            ?>
                                    </div>                                    
                                </li> 
            <?php
        }
        echo '<input type="hidden" value="cwmg" name="activeTab" id="activeTab" >';
        echo '<input type="hidden" value="' . $cwmg->actualvol . '" name="audioId" id="audioId" autocomplete="off" >';
        echo '<input type="hidden" value="' . $cwmgCount . '" name="totalCount" id="totalCount" autocomplete="off" >';
        echo '<input type="hidden" value="' . count($photoReferencesCWMG) . '" name="displayCount" id="displayCount" autocomplete="off" readonly="readonly" >';
    }
    ?>                        
                        <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    </ul>
<?php } ?>


				 <!-- Tribute listing-->   
				 
                <?php if ($activeTab == 'tribute') { ?>    
				<!--<div class="tributesBox">-->
				    <ul class="referenceList wcagPhotoList" >
                    <?php
					$j = 1;
                    if (count($photoReferencesTribute) > 0) {
                        foreach ($photoReferencesTribute as $tribute) {
						 $tribute_idwcag = $tribute->id;

                    if ($j > 4) {
                        $modulo_counter = $j % 4;
                        if ($modulo_counter == 0) {

                            $modulo_counter = 4;
                            $class_id = $modulo_counter;
                            $moduloclass = "tribute_list_3 tribute_list_" . $modulo_counter;
                        } else {
                            $class_id = $modulo_counter;
                            $moduloclass = "tribute_list_" . $class_id;
                        }
                    } else {
                        $class_id = $j;
                        if ($class_id == 4) {
                            $moduloclass = "tribute_list_3 tribute_list_" . $class_id;
                        } else {
                            $moduloclass = "tribute_list_" . $class_id;
                        }
                    }
                            ?>
                                <li class="<?php echo $moduloclass; ?>">
				
                        <div class="tributesGalleryBoxRef">
                        <div class="thumbImg">
								<a href="<?php echo $base_url; ?>/tribute-detail/<?php echo base64_encode($tribute->tributeid); ?>/<?php echo base64_encode($tribute->personid); ?>"title="<?php echo $tribute->firstname . " " . $tribute->lastname; ?>">
                                <?php
                                if ($tribute->personphoto != '') {
                                    $person_filename = $tribute->personphoto;
                                } else {
                                    $person_filename = 'person_nophoto.png';
                                }
                                ?>
                                <img src="<?php echo $tribute_url . $person_filename; ?>" width="141" height="156" alt="<?php echo $tribute->firstname; ?>" title="<?php echo $tribute->firstname; ?>" />
                            </a>
                        </div>
						<div class="content">
                         <h2><?php
                            $tributeName = $tribute->firstname;
                            echo $tributeName;
                            if ($tribute->personprofile != "") {
                                echo "<br /><span>" . $tribute->personprofile . "</span>";
                            }
                            ?>
                        </h2>   
                        <?php
                        $tributetext = strip_tags($tribute->tributetext);
                        if (strlen($tributetext) > 170) {
                            echo '<p>' . substr($tributetext, 0, 170) . '...</p>';
                        } else {
                            echo '<p>' . $tributetext . '</p>';
                        }
                        ?>
                        <span class="paddNone"><a href="<?php echo $base_url; ?>/tribute-detail/<?php echo base64_encode($tribute->tributeid); ?>/<?php echo base64_encode($tribute->personid); ?>"><?php echo t('View More'); ?></a></span>
                    </div>
					</div>
					 </li>
            <?php
			 $j++;
        }
        echo '<input type="hidden" value="tribute" name="activeTab" id="activeTab" >';
    }
    ?>
                       <?php  echo theme('pager', array('element' => 4)); ?>                          
                    </ul>
                    <ul class="referenceList PhotoList" style="display:none;" >
    <?php
	          $count = 1;
    if (count($photoReferencesTribute) > 0) {
        foreach ($photoReferencesTribute as $tribute) {
                
                $tribute_id = $tribute->id;

                if ($count > 4) {
                    $modulo_counter = $count % 4;
                    if ($modulo_counter == 0) {

                        $modulo_counter = 4;
                        $class_id = $modulo_counter;
                        $moduloclass = "tribute_list_3 tribute_list_" . $modulo_counter;
                    } else {
                        $class_id = $modulo_counter;

                        $moduloclass = "tribute_list_" . $class_id;
                    }
                } else {
                    $class_id = $count;
                    if ($class_id == 4) {
                        $moduloclass = "tribute_list_3 tribute_list_" . $class_id;
                    } else {
                        $moduloclass = "tribute_list_" . $class_id;
                    }
                }
                ?>
                <li class="<?php echo $moduloclass; ?>">
				
                        <div class="tributesGalleryBoxRef">
                        <div class="thumbImg">
								<a href="<?php echo $base_url; ?>/tribute-detail/<?php echo base64_encode($tribute->tributeid); ?>/<?php echo base64_encode($tribute->personid); ?>"title="<?php echo $tribute->firstname . " " . $tribute->lastname; ?>">
                                <?php
                                if ($tribute->personphoto != '') {
                                    $person_filename = $tribute->personphoto;
                                } else {
                                    $person_filename = 'person_nophoto.png';
                                }
                                ?>
                                <img src="<?php echo $tribute_url . $person_filename; ?>" width="141" height="156" alt="<?php echo $tribute->firstname; ?>" title="<?php echo $tribute->firstname; ?>" />
                            </a>
                        </div>
						<div class="content">
                         <h2><?php
                            $tributeName = $tribute->firstname;
                            echo $tributeName;
                            if ($tribute->personprofile != "") {
                                echo "<br /><span><h3>" . $tribute->personprofile . "</h3></span>";
                            }
                            ?>
                        </h2>   
                        <?php
                        $tributetext = strip_tags($tribute->tributetext);
                        if (strlen($tributetext) > 170) {
                            echo '<p>' . substr($tributetext, 0, 170) . '...</p>';
                        } else {
                            echo '<p>' . $tributetext . '</p>';
                        }
                        ?>
                        <span class="paddNone"><a href="<?php echo $base_url; ?>/tribute-detail/<?php echo base64_encode($tribute->tributeid); ?>/<?php echo base64_encode($tribute->personid); ?>"><?php echo t('View More'); ?></a></span>
                    </div>
					</div>
					 </li>
                <?php
                $count++;
            
            ?>     
            <?php
        }
		
        echo '<input type="hidden" value="tribute" name="activeTab" id="activeTab" >';
        echo '<input type="hidden" value="' . $tribute->personid . '" name="tributeId" id="tributeId" autocomplete="off" >';
        echo '<input type="hidden" value="' . $tributeCount . '" name="totalCount" id="totalCount" autocomplete="off" >';
        echo '<input type="hidden" value="0" name="displayCount" id="displayCount" autocomplete="off" >';
    }
    ?>
            <div class="facebook_style" id="facebook_style" style='display:none;'><img src="<?php echo $photoImageUrl; ?>loading.gif" alt="Loading" /></div>                                
                    </ul>

<?php } ?>
<!--</div>-->

                <input type="hidden" name="photo_id" id="photo_id" value="<?php echo base64_decode(arg(1)); ?>" readonly="readonly" />
                <input type="hidden" name="base_url" id="base_url" value="<?php echo $base_url; ?>" readonly="readonly" />
                <input type="hidden" name="photo_path" id="photo_path" value="<?php echo variable_get('ghp_photos_path'); ?>" readonly="readonly" />
                <input type="hidden" name="cartoon_path" id="cartoon_path" value="<?php echo $cartoon_medium_url; ?>" readonly="readonly" />
                <input type="hidden" name="poster_path" id="poster_path" value="<?php echo $poster_medium_path; ?>" readonly="readonly" />
                <input type="hidden" name="poster_path_main" id="poster_path_main" value="<?php echo $poster_main_image_path; ?>" readonly="readonly" />
                <input type="hidden" name="stamp_path" id="stamp_path" value="<?php echo $stamp_url; ?>" readonly="readonly" />
				<input type="hidden" name="tribute_path" id="tribute_path" value="<?php echo $tribute_url; ?>" readonly="readonly" />
                <input type="hidden" name="audio_url" id="audio_url" value="<?php echo $get_audio_path; ?>" readonly="readonly" />
				<input type="hidden" name="tribute_url" id="tribute_url" value="<?php echo $tribute_url; ?>" readonly="readonly" />
                <div class="btnHolder"></div>
                </div>
            <div class="referenceContentBot"></div>
        </div>
    </div>


	
	