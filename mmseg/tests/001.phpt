--TEST--
Check for mmseg presence
--SKIPIF--
<?php if (!extension_loaded("mmseg")) print "skip"; ?>
--FILE--
<?php 
$arr = mmseg_segment("你好，世界");
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
