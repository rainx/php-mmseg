--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
$arr = mmseg_segment("你好，世界");
echo count($arr);
?>
--EXPECT--
2
