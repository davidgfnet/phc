<?
	$x = "a";
	$a = 1;
	$b =& $$x;
	$b = 2;
	echo $a;
?>