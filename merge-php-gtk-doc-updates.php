#!/usr/local/bin/php
<?php
define ('MAX_UPDATES_TO_SHOW', 15);

/**
 * Class for combining current list of updates with
 * a previous list of updates.
 *
 * The updated files should be listed as a set of
 * HTML links. The resulting file will be an HTML
 * fragment that consists of a div block a header
 * and an unordered list of HTML links.
 *
 * @author Scott Mattocks
 */
class ManualUpdates {
  
  /**
   * The file that contains updates from the last time.
   * Also where the new updates will be written to.
   * @var string
   */
  var $filenameOld;
  /**
   * The file that contains tonights updated classes.
   * @var string
   */
  var $filenameNew;
  var $filenameOut;
  /**
   * The classes that were updated previously.
   * @var array
   */
  var $oldUpdates = array();
  /**
   * The classes that were just updated.
   * @var array
   */
  var $newUpdates = array();
  /**
   * Which array is to updated
   * @var string
   */
  var $oldNew;
  /**
   * Updates to be written to file.
   * @var array
   */
  var $updatesToWrite = array();
  /**
   * The current XML tag.
   * @var string
   */
  var $currentTag = '';
  /**
   * The current link.
   * @var string
   */
  var $currentLink = '';
  
  /**
   * Constructor. 
   * Sets files names
   * 
   * @access protected
   * @param  none
   * @return void
   */
  function ManualUpdates($oldFile = NULL, $newFile = NULL, $outFile = NULL) {
    
    $this->filenameOld = $oldFile;
    $this->filenameNew = $newFile;
    $this->filenameOut = $outFile;
  }
  /**
   * Get the updated classes from the old file. 
   * Sets oldUpdates.
   * 
   * @access public
   * @param  none
   * @return void
   */
  function getOldUpdates() {
    
    $this->oldNew = 'old';    
    $this->_getUpdates($this->filenameOld);
  }
  /**
   * Get the newly updated classes from the new file.
   * Sets newUpdates.
   * 
   * @access public
   * @param  none
   * @return void
   */
  function getNewUpdates() {

    $this->oldNew = 'new';
    $this->_getUpdates($this->filenameNew);
  }
  /**
   * Trim the updates to the either the size limit or the end of 
   * today's updates. Which ever is greater.
   * 
   * @access public
   * @param  none
   * @return void
   */
  function trimUpdates() {
    
    // We want to show MAX_UPDATES_TO_SHOW or the total of
    // tonight's updates, which ever is greater.
    $endOfUpdates = max(MAX_UPDATES_TO_SHOW, count($this->newUpdates));
    $this->updatesToWrite = array_merge($this->oldUpdates, $this->newUpdates);
	uasort($this->updatesToWrite, array(&$this, 'sortByDate'));
    $this->updatesToWrite = array_slice($this->updatesToWrite, 0, $endOfUpdates);
  }

  function sortByDate($a, $b)
  {
	  if ((int)$a[1] == (int)$b[1]) {
		  return strcmp($a[0], $b[0]);
	  }

	  return ((int)$a[1] < (int)$b[1]) ? 1 : -1;
  }

  /**
   * Write the updates to the file.
   * Updates are written back to filenameOld
   * 
   * @access public
   * @param  none
   * @return void
   */
  function writeUpdatesToFile() {

    // Open the file for writing
    $fp = fopen($this->filenameOut, 'w');
    
    // Write the begining information
    $this->_writeBegining($fp);

    // Write each updated class/file
    foreach ($this->updatesToWrite as $url => $classinfo) {
      $this->_writeClass($fp, $classinfo, $url);
    }

    // Write the end of the file
    $this->_writeEnd($fp);
    
    // Close the file
    fclose($fp);
  }
  /**
   * Write the begining of the file.
   * For now it is just a div, header, paragraph and ul
   *
   * @access protected
   * @param  resource  &$filePointer The file to write to
   * @return none
   */
  function _writeBegining(&$filePointer) {
   
    $text = '<p>' . "\n";
    fwrite($filePointer, $text);
  }

  /**
   * Write one item to the file.
   * This includes a list itema and a link.
   *
   * @access protected
   * @param  resource  &$filePointer The file to write to
   * @param  string    $classname    The text to link
   * @param  string    $url          The url to link to
   * @return none
   */
  function _writeClass(&$filePointer, $classinfo, $url) {
    
    $line = '&#0149; <a href="' . $url . '" date="' . $classinfo[1] . '">' . $classinfo[0] . '</a><br />' . "\n";
    fwrite($filePointer, $line);
  }
  /**
   * Write the end of the file.
   * For now it is just a ul, paragraph and a div
   *
   * @access protected
   * @param  resource  &$filePointer The file to write to
   * @return none
   */
  function _writeEnd(&$filePointer) {

    $text = '</p>';
    fwrite($filePointer, $text);
  }
  /**
   * Method that actually gets info from the file.
   * 
   * @access protected
   * @param  string    $filename
   * @return &array
   */
  function &_getUpdates(&$filename) {

    // Check for the file first.
    if (!is_readable($filename)) {
      trigger_error('Cannot get updates from ' . $filename, E_USER_WARNING);
      trigger_error($filename . ' does not exist or is not readable by this user.', E_USER_ERROR);
      exit;
    }

    // Read in the file's contents and parse them as xml
    $this->_parseXML(file_get_contents($filename));
  }
  /**
   * Parse a chunk of xml.
   * 
   * @access protected
   * @param  string    $xml
   * @return array
   */
  function _parseXML($xml) {
   
    $xmlParser = xml_parser_create();
    xml_set_element_handler($xmlParser, array(&$this, '_startElementHandler'), array(&$this, '_endElementHandler'));
    xml_set_character_data_handler($xmlParser, array(&$this, '_characterDataHandler'));
    xml_parse($xmlParser, $xml);
  }
  /**
   * XML begining element handler.
   *
   * @access protected
   * @param  object    $parser
   * @param  string    $name
   * @param  array     $attr
   * @return void
   */  
  function _startElementHandler($parser, $name, $attr) {
    
    // If this is a link we want put the url in the array.
    if (strcasecmp($name, 'a') === 0) {
      $this->currentLink = $attr['HREF'];
	  $this->currentDate = isset($attr['DATE']) ? $attr['DATE'] : strftime('%Y%m%d');
    }

    // Save the tag so that we know where we are later.
    $this->currentTag = $name;
  }
  /**
   * XML end element handler.
   *
   * @access protected
   * @param  object    $parser
   * @param  string    $name
   * @return void
   */
  function _endElementHandler($parser, $name) {
	$this->currentTag = '';

    // I don't think there is anything to do here.
  }
  /**
   * XML character data handler.
   *
   * @access protected
   * @param  object    $parser
   * @param  string    $data
   * @return void
   */
  function _characterDataHandler($parser, $data) {

    // Check to see if we are in an anchor tag.
    if (strcasecmp($this->currentTag, 'a') === 0 && $data !== "\n") {
      // We are so add the link to the array.
      $updates = $this->oldNew . 'Updates';
      $this->{$updates}[$this->currentLink] = array($data, $this->currentDate);
    }
  }
}

// Check for files passed as arguments.
if (isset($argv[1])) {
  $oldfile = $argv[1];
} else {
  $oldfile = 'updates.php';
}

if (isset($argv[2])) {
  $newfile = $argv[2];
} else {
  $newfile = 'new_updates.php';
}

if (isset($argv[3])) {
  $outfile = $argv[3];
} else {
  $outfile = $oldfile;
}

// Create the current list of updates.
$mu =& new ManualUpdates($oldfile, $newfile, $outfile);
$mu->getOldUpdates();
$mu->getNewUpdates();
$mu->trimUpdates();
$mu->writeUpdatesToFile();
?>
