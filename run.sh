declare -a bench=("barnes" "cholesky" "fft" "fmm" "radix" "radiosity" "raytrace" "lu.cont" "lu.ncont" "ocean.cont" "ocean.ncont"  "water.nsq" "water.sp" "barnes-scale");

str1=" -p splash2-"
str3=" -i large -n 2 -c prakash -d ./prak_"


for i in "${bench[@]}"
do
	str="$str1$i$str3$i"
   	./run-sniper $str
   # or do whatever with individual element of the array
done
