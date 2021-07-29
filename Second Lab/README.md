### Results of the execution are shown below (click to expand). Complete results you can find in Results Screenshots folder.

<p align="center">
  <img src="https://i.ibb.co/RDGsg7k/tsf.png" title="ProducerConsumer" width = "200" height="200">
  <img src="https://i.ibb.co/PwVg9WL/pc1.png" title="ThreadSafeCounter" width = "200" height="200">
</p>

You can find an amazing talk on atomics in C++ here: [CppCon 2017: Fedor Pikus “C++ atomics, from basic to advanced. What do they really do?”](https://www.youtube.com/watch?v=ZQFzMfHIxng&t=2173s). Fedor is a great speaker and nicely explains atomics in C++. He provides benchmarks that show that lock-free does not always mean faster. It all depends on your implementation and your program needs. </br>
And two more Youtube links: 
1. [CppCon 2017: Ansel Sermersheim “Multithreading is the answer. What is the question? (part 1 of 2)”](https://youtu.be/GNw3RXr-VJk). Ansel goes over multithreading in general and gives a great example on how to correctly use mutexes. The second part of his talk is a bit more complicated and I think is harder to understand for beginners in multithreading.
2. [Back to Basics: Concurrency - Arthur O'Dwyer - CppCon 2020](https://youtu.be/F6Ipn7gCOsY). This is also a good video about concurrency basics.


