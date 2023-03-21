echo "working directory:"
read directory

if [ -z ㅁ"$directory" ]
then
	cd
else     
	cd $directory  
	if [ $? -ne 0 ]  
	then
		echo "Error"
		exit 0
	fi
fi

for i in *
do
	if [ -f $i ]
	then
	mv $i `echo $i | tr '[A-Z][a-z]' '[a-z][A-Z]'`
	fi
done
exit 0
