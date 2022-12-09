#!/bin/bash

#Author: Ježek


echo "--------INFO--------"
echo "Testy pomoci diff porovnaji vystup vzoroveho reseni a vaseho. Jinymi slovy - kdyz se nic nevypise, je to dobre."
echo ""
#g++ -g aut.c -o aut -pthread #compilation

echo "--------Testy-------"
echo ""

P="/usr/local/groups/verifikace/IPS-projekt2"

echo "--------1--------"
echo "test: aut 1 \"tak j\" 1 "
$P/aut 1 "tak j" 1 <$P/zadani.txt >test_right.out
 ./aut 1 "tak j" 1 <$P/zadani.txt >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------2--------"
echo "test: aut 1 \"Alias\" 1 \"FILES\" -1"
$P/aut 1 "Alias" 1 "FILES" -1 <$P/sudoers >test_right.out
 ./aut 1 "Alias" 1 "FILES" -1 <$P/sudoers >test_mine.out
 echo "diff:"
diff test_mine.out test_right.out

echo "--------3--------"
echo "test: aut 1 \"Alias\" 1 \"FILES\" -1 \"mount\" -1 \"chgrp\" -1 \"Host \" -1"
$P/aut 1 "Alias" 1 "FILES" -1 "mount" -1 "chgrp" -1 "Host " -1 <$P/sudoers >test_right.out
 ./aut 1 "Alias" 1 "FILES" -1 "mount" -1 "chgrp" -1 "Host " -1 <$P/sudoers >test_mine.out
 echo "diff:"
diff test_mine.out test_right.out

echo "--------4--------"
echo "test: aut 1 \"env_k\" 1 \"root \" 1"
$P/aut 1 "env_k" 1 "root " 1 <$P/sudoers >test_right.out
 ./aut 1 "env_k" 1 "root " 1 <$P/sudoers >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------5--------"
echo "test: aut 2 \"env_k\" 1 \"root \" 1"
$P/aut 2 "env_k" 1 "root " 1 <$P/sudoers >test_right.out
 ./aut 2 "env_k" 1 "root " 1 <$P/sudoers >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------6--------"
echo "test: aut 1 \"(ALL)\" 1 \"rogal\" -1"
$P/aut 1 "(ALL)" 1 "rogal" -1 <$P/sudoers >test_right.out
 ./aut 1 "(ALL)" 1 "rogal" -1 <$P/sudoers >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------7--------"
echo "test: aut 1 \"(ALL)\" 1 \"wheel\" -1"
$P/aut 1 "(ALL)" 1 "wheel" -1 <$P/sudoers >test_right.out
 ./aut 1 "(ALL)" 1 "wheel" -1 <$P/sudoers >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------8--------"
echo "test: aut 2 \"Alias\" 1 \"SERVE\" 1"
$P/aut 2 "Alias" 1 "SERVE" 1 <$P/sudoers >test_right.out
 ./aut 2 "Alias" 1 "SERVE" 1 <$P/sudoers >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------9--------"
echo "test: aut 2 \"root:\" 1 \"rogal\" 1"
$P/aut 2 "root:" 1 "rogal" 1 <$P/group >test_right.out
 ./aut 2 "root:" 1 "rogal" 1 <$P/group >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------10-------"
echo "test: aut 1 \"root:\" 1 \"rogal\" 1"
$P/aut 1 "root:" 1 "rogal" 1 <$P/group >test_right.out
 ./aut 1 "root:" 1 "rogal" 1 <$P/group >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------11-------"
echo "test: aut 2 \"root:\" 1 \"root,\" 1 \"rogal\" 1"
$P/aut 2 "root:" 1 "root," 1 "rogal" 1 <$P/group >test_right.out
 ./aut 2 "root:" 1 "root," 1 "rogal" 1 <$P/group >test_mine.out
echo "diff:"
diff test_mine.out test_right.out

echo "--------12-------"
echo "test: aut 2 \"root,\" 1 \"root:\" 1 \"rogal\" 1"
$P/aut 2 "root," 1 "root:" 1 "rogal" 1 <$P/group >test_right.out
 ./aut 2 "root," 1 "root:" 1 "rogal" 1 <$P/group >test_mine.out
echo "diff:"
diff test_mine.out test_right.out


# cleanup
rm test_mine.out test_right.out #aut

