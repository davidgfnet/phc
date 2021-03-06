<?php

function test_isset($base) {
    $a = str_repeat('xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx', 1000);    
    
    $t = $base;
    test_start(__FUNCTION__);
    do {
	if (!isset($a)) {
	    test_regression(__FUNCTION__);
	}
    } while (--$t !== 0);

    return test_end(__FUNCTION__);
}

function test_isset_enabled() {
    return TRUE;
}

?>
