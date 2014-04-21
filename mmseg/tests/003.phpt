--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
$mmseg = mmseg_open("/opt/mmseg/etc");
$arr = mmseg_segment($mmseg, "你好，世界");
mmseg_close($mmseg);
var_dump($arr);
?>
--EXPECT--
array(3) {
  [0]=>
  string(6) "你好"
  [1]=>
  string(3) "，"
  [2]=>
  string(6) "世界"
}
