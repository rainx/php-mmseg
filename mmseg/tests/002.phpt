--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
$obj = mmseg_open("/opt/etc");
var_dump($obj);
?>
--EXPECT--
resource(4) of type (mmseg segmenter manager resource)
