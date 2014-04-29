--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
@unlink(dirname(__FILE__) . "/" . "synonyms.dat");
$arr = mmseg_gensynonyms(dirname(__FILE__) . "/" . "synonyms.txt", dirname(__FILE__) . "/" . "synonyms.dat" );
if (is_file( dirname(__FILE__) . "/" . "synonyms.dat")) {
    echo 1;
}
?>
--EXPECT--
1
