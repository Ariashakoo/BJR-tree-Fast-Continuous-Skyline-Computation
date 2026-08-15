
# BJR-tree: Fast Continuous Skyline Computation

This repository provides a clean and efficient C++ implementation of the **Balanced Jointed Rooted Tree (BJR-tree)** algorithm and the **Non-Dominated Relation Cache (ND-cache)**, as described in the paper:

> **"BJR-tree: fast skyline computation algorithm using dominance relation-based tree structure"**  
> Kenichi Koizumi, Peter Eades, Kei Hiraki, Mary Inaba  
> *International Journal of Data Science and Analytics*, 2019  
> DOI: [10.1007/s41060-018-0098-x](https://doi.org/10.1007/s41060-018-0098-x)

The BJR-tree is designed for **continuous skyline computation** in dynamic, streaming environments—ideal for real-time applications like high-throughput single-cell screening, fraud detection, or sensor data analysis.

---

## 📌 What is Skyline Computation?

In a multi-dimensional dataset, a **skyline point** is one that is *not dominated* by any other point. A point `A` dominates point `B` if `A` is better than or equal to `B` in all dimensions and strictly better in at least one.

Skyline computation extracts these "best" or "extreme" points without requiring user-defined weights—making it perfect for exploratory data analysis and serendipitous discovery.

---

## 🚀 Features

- ✅ **BJR-tree**: A compact tree structure that encodes dominance relations with only **O(n)** edges (vs. O(n²) in naive approaches).
- ✅ **Lazy injection & balancing**: Optimizes tree depth and structure for faster updates.
- ✅ **ND-cache**: Caches non-dominated relationships between skyline points to avoid redundant comparisons—especially useful when skyline points reappear after temporary deactivation.
- ✅ **Dimensionality-independent**: Performance does not degrade with increasing dimensions (unlike R-trees or quadtrees).
- ✅ **Real-time ready**: Tested on synthetic and real-world biological datasets (e.g., blood cells, Euglena) with **up to 70× speedup** over LookOut.

---

## 🛠️ Installation

### Prerequisites
- C++17 compiler (e.g., GCC ≥ 7 or Clang ≥ 6)
- CMake ≥ 3.10
- (Optional) Google Test for unit tests

### Build Instructions
```bash
git clone https://github.com/Ariashakoo/DS-Project.git
cd DS-Project
mkdir build && cd build
cmake ..
make
```

The main executable will be `BJRTree`.

---

## 📂 Project Structure

```
DS-Project/
├── src/               # Core implementation
│   ├── BJRTree.cpp   
│   ├── Node.h 
│   └── BJRTree.h    
├── include/           # Header files
├── data/              # Sample datasets (synthetic & real)
├── tests/             # Unit and performance tests
├── CMakeLists.txt
└── README.md
```

---

## 📊 Performance

On synthetic datasets (320k points):
- **2D**: ~3× faster than LookOut  
- **8D**: **>70× faster** than LookOut  

On real blood cell data (4D, 546 variants):
- Average runtime: **14.2 µs** (vs. 33.8 µs for LookOut)

---

## 📚 References

- Koizumi, K., Eades, P., Hiraki, K. et al. BJR-tree: fast skyline computation algorithm using dominance relation-based tree structure. *Int J Data Sci Anal* **7**, 17–34 (2019). https://doi.org/10.1007/s41060-018-0098-x

---

## 🤝 Contributing

Contributions are welcome! Please open an issue or submit a pull request for:
- Performance optimizations
- Python bindings
- Additional dataset loaders
- Parallel/multi-threaded extensions

---

> **Note**: This implementation is for educational and research purposes. For production use in biomedical or industrial systems, please validate against domain-specific requirements.
