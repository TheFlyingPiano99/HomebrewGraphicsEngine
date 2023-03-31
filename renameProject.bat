call :strLen %1% strlen
if strlen > 2
	 (gc HomebrewGraphicsEngine.sln) -replace 'HomebrewGraphicsEngine', %1% | Out-File -encoding ASCII %1%.sln
else
	echo "Too short solution name!"
"