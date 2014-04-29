--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
@unlink(dirname(__FILE__) . "/" . "thesaurus.dat");
$arr = mmseg_genthesaurus(dirname(__FILE__) . "/" . "thesaurus.txt", dirname(__FILE__) . "/" . "thesaurus.dat" );
if (is_file( dirname(__FILE__) . "/" . "thesaurus.dat")) {
    echo 1;
}
?>
--EXPECT--
1
