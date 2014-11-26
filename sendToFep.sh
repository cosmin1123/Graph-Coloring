
if [ "$#" -ne 1 ] ; then
  echo "Usage: 'sh $0 <Fep_Name>'" >&2
  exit 1
fi

name="$1"

scp -r src $name@fep.grid.pub.ro:AppGraphColoring

ssh $name@fep.grid.pub.ro 'cd AppGraphColoring && module load compilers/gnu-4.7.0 && make build_run'
