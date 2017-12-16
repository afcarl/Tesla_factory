num_cars=1
num_space=1
num_workers=2
./tesla_factory.out $num_cars $num_space $num_workers

num_workers=8
./tesla_factory.out $num_cars $num_space $num_workers

num_cars=4
num_space=8
num_workers=8
./tesla_factory.out $num_cars $num_space $num_workers

num_workers=16
./tesla_factory.out $num_cars $num_space $num_workers

num_workers=32
./tesla_factory.out $num_cars $num_space $num_workers

num_workers=37
./tesla_factory.out $num_cars $num_space $num_workers
