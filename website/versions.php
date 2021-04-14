<?php
        function file_crc($file) {
                $file_string = file_get_contents($file);
                $crc = crc32($file_string);
                return sprintf("%u", $crc);
        }

        $dir = ".";
        $cdir = scandir($dir);

        foreach ($cdir as $key => $value) {
                if (!in_array($value,array(".","..", "versions.php"))) {
                        if (!is_dir($dir . DIRECTORY_SEPARATOR . $value)) {
                                echo $value . " " . file_crc($value) . "\n";
                        }
                }
        }
?>