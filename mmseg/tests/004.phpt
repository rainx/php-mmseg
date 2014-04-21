--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
@unlink(dirname(__FILE__) . "/" . "unigram.txt.uni");
$arr = mmseg_gendict(dirname(__FILE__) . "/" . "unigram.txt", dirname(__FILE__) . "/" . "unigram.txt.uni" );
if (is_file( dirname(__FILE__) . "/" . "unigram.txt.uni")) {
    echo 1;
}
?>
--EXPECT--
1
