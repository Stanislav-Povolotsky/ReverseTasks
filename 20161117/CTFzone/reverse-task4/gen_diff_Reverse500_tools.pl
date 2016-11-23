open(F,"<Reverse500.exe");
binmode(F);
read(F, $data, 1000000);
close(F);

$patch_ofs = 0x01ACAE;
$data1 = substr($data, 0, $patch_ofs);
$data2 = substr($data, $patch_ofs + 1, length($data));
for($pos =0; $pos < 256; ++$pos)
{
 open(F,">Reverse500.$pos.exe");
 binmode(F);
 print F $data1 . chr($pos) . $data2;
 close(F);
}
