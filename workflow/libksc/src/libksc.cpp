#include <array>
#include <tuple>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#include <nanobind/nanobind.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/string.h>
#include <nanobind/eigen/dense.h>

//------------------------------------------------------------------------------------------------
namespace libksc {
    typedef Eigen::Matrix<uint8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXui8;

    //------------------------------------------------------------------------------------------------
    namespace terrain_dat {
        //------------------------------------------------------------------------------------------------
        struct Header {
            int x_min, x_size, z_min, z_size;
        };

        //------------------------------------------------------------------------------------------------
        std::tuple<Header, MatrixXui8> load(std::string &path) {
            std::ifstream stream;
            stream.open(path, std::ios_base::binary);
            Header header;
            stream.read(reinterpret_cast<char*>(&header), sizeof(header));
            int n_elements;
            stream.read(reinterpret_cast<char*>(&n_elements), sizeof(n_elements));
            MatrixXui8 data = MatrixXui8::Zero(header.x_size, header.z_size);

            for (int i = 0; i < n_elements; i++) {
                int x, z;
                stream.read(reinterpret_cast<char*>(&x), sizeof(x));
                stream.read(reinterpret_cast<char*>(&z), sizeof(z));
                data(x - header.x_min, z - header.z_min) = 1;
            }

            stream.close();
            return {header, data};
        }

        //------------------------------------------------------------------------------------------------
        void save(std::string &path, Header &header, MatrixXui8 &data) {
            std::ofstream stream;
            stream.open(path, std::ios_base::binary);
            stream.write(reinterpret_cast<char*>(&header), sizeof(header));

            std::vector<std::tuple<int, int>> doublets;
            doublets.reserve(data.size());
            int n_elements = 0;

            for (int i = 0; i < data.rows(); i++) {
                for (int j = 0; j < data.cols(); j++) {
                    float value = data(i, j);

                    if (value == 0)
                        continue;
                    
                    doublets.push_back({i + header.x_min, j + header.z_min});
                    n_elements++;
                }
            }

            stream.write(reinterpret_cast<char*>(&n_elements), sizeof(n_elements));
            
            for (auto [x, z] : doublets) {
                stream.write(reinterpret_cast<char*>(&x), sizeof(x));
                stream.write(reinterpret_cast<char*>(&z), sizeof(z));
            }

            stream.close();
        }

        //------------------------------------------------------------------------------------------------
        void save(std::string &path, Header &header, Eigen::MatrixXf &data) {
            std::ofstream stream;
            stream.open(path, std::ios_base::binary);
            stream.write(reinterpret_cast<char*>(&header), sizeof(header));

            std::vector<std::tuple<int, int, float>> triplets;
            triplets.reserve(data.size());
            int n_elements = 0;

            for (int i = 0; i < data.rows(); i++) {
                for (int j = 0; j < data.cols(); j++) {
                    float value = data(i, j);

                    if (std::isnan(value))
                        continue;
                    
                    triplets.push_back({i + header.x_min, j + header.z_min, value});
                    n_elements++;
                }
            }

            stream.write(reinterpret_cast<char*>(&n_elements), sizeof(n_elements));
            
            for (auto [x, z, value] : triplets) {
                stream.write(reinterpret_cast<char*>(&x), sizeof(x));
                stream.write(reinterpret_cast<char*>(&z), sizeof(z));
                stream.write(reinterpret_cast<char*>(&value), sizeof(value));
            }

            stream.close();
        }

/*
        namespace {
            std::array<int, 2> _read_header(std::ifstream &stream) {
                std::string header;
                std::getline(stream, header);
                int i_min, i_step, i_max, j_min, j_step, j_max;
                std::sscanf(header.c_str(), "(%i,%i,%i|%i,%i,%i)", &i_min, &i_step, &i_max, &j_min, &j_step, &j_max);
                return std::array<int, 2>{(i_max - i_min + 1) / i_step, (j_max - j_min + 1) / j_step};
            }
        }

        MatrixXui8 load(std::string &path) {
            std::ifstream stream;
            stream.open(path);
            auto [i_max, j_max] = _read_header(stream);
            MatrixXui8 data(i_max, j_max);

            char c;
            for (int i = 0; i < i_max; i++) {
                for (int j = 0; j < j_max; j++) {
                    
                    stream.get(c);
                    data(i, j) = c - '0';
                }

                // Skip newline
                stream.get(c);
            }

            stream.close();
            return data;
        }
*/
    }

    //------------------------------------------------------------------------------------------------
    namespace clustering {
        //------------------------------------------------------------------------------------------------
        struct BinaryImage_DBSCAN {
            float eps_sq;
            int eps_ceiled;
            int min_samples;
            int n_labels_;
            Eigen::MatrixXi labels_;

            //------------------------------------------------------------------------------------------------
            BinaryImage_DBSCAN(float _eps = 1.5, int _min_samples = 0) : min_samples(_min_samples) {
                eps_sq = std::pow(_eps, 2);
                eps_ceiled = std::ceil(_eps);
            }

            //------------------------------------------------------------------------------------------------
            BinaryImage_DBSCAN fit(MatrixXui8 &X, void* y = nullptr) {
                labels_ = Eigen::MatrixXi(X.rows(), X.cols());
                labels_.fill(-1);
                n_labels_ = 0;

                for (int i = 0; i < X.rows(); i++) {
                    for (int j = 0; j < X.cols(); j++) {
                        _handle_pixel({i, j}, X);
                    }
                }

                return *this;
            }

            //------------------------------------------------------------------------------------------------
            void _handle_pixel(std::array<int, 2> pixel_pos, MatrixXui8 &X) {
                auto [i, j] = pixel_pos;

                if (X(i, j) == 0)
                    return;

                if (labels_(i, j) >= 0)
                    return;
                
                bool initial_pixel_assigned = false;
                std::queue<std::array<int, 2>> samples_to_process;
                samples_to_process.push(pixel_pos);

                while (!samples_to_process.empty()) {
                    pixel_pos = std::move(samples_to_process.front()); 
                    samples_to_process.pop();
                    std::vector<std::array<int, 2>> neighbors = _get_neighbors(pixel_pos, X);

                    if (neighbors.size() < min_samples)
                        continue;

                    for (std::array<int, 2> &neighbor : neighbors) {
                        if (labels_(neighbor[0], neighbor[1]) >= 0)
                            continue;
                        
                        labels_(neighbor[0], neighbor[1]) = n_labels_;
                        samples_to_process.push(neighbor);
                    }

                    if (!initial_pixel_assigned) {
                        labels_(pixel_pos[0], pixel_pos[1]) = n_labels_;
                        initial_pixel_assigned = true;
                    }
                }

                n_labels_++;
            }

            //------------------------------------------------------------------------------------------------
            Eigen::MatrixXi fit_predict(MatrixXui8 &X, void* y = nullptr) {
                fit(X);
                return labels_;
            }

            //------------------------------------------------------------------------------------------------
            std::vector<std::array<int, 2>> _get_neighbors(std::array<int, 2> pixel_pos, MatrixXui8 &X) {
                auto [i_center, j_center] = pixel_pos;
                std::vector<std::array<int, 2>> neighbors;

                for (int i = i_center - eps_ceiled; i <= i_center + eps_ceiled; i++) {
                    if (i < 0 || i >= X.rows())
                        continue;

                    for (int j = j_center - eps_ceiled; j <= j_center + eps_ceiled; j++) {
                        if (j < 0 || j >= X.cols())
                            continue;

                        if (X(i, j) == 0)
                            continue;

                        if (i == i_center && j == j_center)
                            continue;
                        
                        if (std::pow(i - i_center, 2) + std::pow(j - j_center, 2) > eps_sq)
                            continue;
                        
                        neighbors.push_back({i, j});
                    }
                }

                return neighbors;
            }
        };
    }

    //------------------------------------------------------------------------------------------------
    Eigen::MatrixXf _window_to_features(Eigen::MatrixXf &image) {
        // Allocate maximum possible size
        Eigen::MatrixXf features(image.rows() * image.cols(), 2);
        features.conservativeResize(0, 2);
        float i_center = (image.rows() + 1) / 2;
        float j_center = (image.cols() + 1) / 2;

        for (int i = 0; i < image.rows(); i++) {
            for (int j = 0; j < image.cols(); j++) {
                if (image(i, j) == 0)
                    continue;

                features.conservativeResize(features.rows() + 1, features.cols());
                features(features.rows() - 1, 0) = i - i_center;
                features(features.rows() - 1, 1) = j - j_center;
            }
        }

        return features;
    }

    //------------------------------------------------------------------------------------------------
    //! Returns a matrix of the same size as the input binary skeleton image, where all non-zero elements
    //! have an angle from (-PI/2, PI/2] assigned for the local direction of the skeleton. Othere elements
    //! get nan assigned.
    //!
    //! The underlining algorithm is a PCA on a window centered on the pixel to analyze. The direction is
    //! given by the tangent angle of the elements of the first principal component
    Eigen::MatrixXf skeleton_dir(Eigen::MatrixXf &image, int window_size = 11)
    {
        int offset = (window_size - 1) / 2;
        int i_max = image.rows();
        int j_max = image.cols();
        Eigen::MatrixXf out(i_max, j_max);
        out.fill(std::nanf("nan"));

        for (int i = 0; i < i_max; i++) {
            int i_win_start = i - offset;

            if (i_win_start < 0 || i_win_start + window_size >= i_max)
                continue;

            for (int j = 0; j < j_max; j++) {
                int j_win_start = j - offset;

                if (j_win_start < 0 || j_win_start + window_size >= j_max)
                    continue;

                if (image(i, j) == 0)
                    continue;

                Eigen::MatrixXf window = image.block(i_win_start, j_win_start, window_size, window_size);
                Eigen::MatrixXf features = _window_to_features(window);

                if (features.rows() < 2)
                    continue;

                // Get principal components
                Eigen::JacobiSVD<Eigen::MatrixXf> svd(features, Eigen::ComputeThinV);
                Eigen::MatrixXf V = svd.matrixV();

                if (V(0, 1) == 0) {
                    out(i, j) = M_PI / 2;
                } else {
                    out(i, j) = -std::atan(V(0, 0) / V(0, 1));
                }
                
            }
        }

        return out;
    }

    //------------------------------------------------------------------------------------------------
    Eigen::MatrixXf patches_dir(MatrixXui8 &image) {
        int i_max = image.rows();
        int j_max = image.cols();

        // Create for each label a list of indices of all associated pixels
        Eigen::MatrixXi labels = clustering::BinaryImage_DBSCAN(1.5, 1).fit_predict(image);
        int n_labels = 1 + labels.array().maxCoeff();
        std::vector<Eigen::MatrixXf> patches;
        patches.reserve(n_labels);

        for (int i = 0; i < n_labels; i++) {
            Eigen::MatrixXf patch(i_max * j_max, 2);
            patch.conservativeResize(0, 2);
            patches.push_back(patch);
        }

        for (int i = 0; i < i_max; i++) {
            for (int j = 0; j < j_max; j++) {
                int label = labels(i, j);

                if (label < 0)
                    continue;
                
                Eigen::MatrixXf &patch = patches[label];
                patch.conservativeResize(patch.rows() + 1, patch.cols());
                patch(patch.rows() - 1, 0) = i;
                patch(patch.rows() - 1, 1) = j;
            }
        }

        // Create output matrix that contains the angles
        Eigen::MatrixXf out(i_max, j_max);
        out.fill(std::nanf("nan"));

        for (Eigen::MatrixXf &patch : patches) {
            if (patch.rows() < 2)
                continue;

            // The features are pixel position vectors relative to the centroid
            Eigen::MatrixXf features(patch);

            for (int j = 0; j < 2; j++) {
                features.col(j).array() -= features.col(j).array().mean();
            }

            // Get direction from principal components
            Eigen::JacobiSVD<Eigen::MatrixXf> svd(features, Eigen::ComputeThinV);
            Eigen::MatrixXf V = svd.matrixV();

            float dir;

            if (V(0, 1) == 0) {
                dir = M_PI / 2;
            } else {
                dir = std::atan(V(0, 0) / V(0, 1));
            }

            for (int i = 0; i < patch.rows(); i++) {
                out(static_cast<int>(patch(i, 0)), static_cast<int>(patch(i, 1))) = dir;
            }
        }

        return out;
    }
}

/*
def patches_angles(image):
    angles = np.full(image.shape, np.nan)
    labels = BinaryImage_DBSCAN(eps=1.5, min_samples=1).fit_predict(image.astype(np.uint8))

    for label in tqdm(list(range(labels.max() + 1)), leave=False):
        i_list, j_list = np.where(labels == label)

        try:
            i_list -= i_list.min()
            j_list -= j_list.min()
        except ValueError:
            continue

        window = np.zeros((i_list.max() + 1, j_list.max() + 1))
        window[i_list, j_list] = 1
        _, eig_vecs = np.linalg.eigh(inertia_tensor(window))

        if eig_vecs[0, 1] == 0:
            angles[labels == label] = np.pi / 2
        else:
            angles[labels == label] = -np.arctan(eig_vecs[0, 0] / eig_vecs[0, 1])
    
    return angles
}
*/

namespace nb = nanobind;

NB_MODULE(_libksc, m) {
    m.def("load_terrain_dat", &libksc::terrain_dat::load);
    m.def("save_terrain_dat", nb::overload_cast<std::string&, libksc::terrain_dat::Header&, libksc::MatrixXui8&>(&libksc::terrain_dat::save));
    m.def("save_terrain_dat2", nb::overload_cast<std::string&, libksc::terrain_dat::Header&, Eigen::MatrixXf&>(&libksc::terrain_dat::save));
    m.def("skeleton_dir", &libksc::skeleton_dir);
    m.def("patches_dir", &libksc::patches_dir);

    nb::class_<libksc::terrain_dat::Header>(m, "Header")
        .def_rw("x_min", &libksc::terrain_dat::Header::x_min)
        .def_rw("x_size", &libksc::terrain_dat::Header::x_size)
        .def_rw("z_min", &libksc::terrain_dat::Header::z_min)
        .def_rw("z_size", &libksc::terrain_dat::Header::z_size)
    ;

    nb::class_<libksc::clustering::BinaryImage_DBSCAN>(m, "BinaryImage_DBSCAN")
        .def_rw("labels_", &libksc::clustering::BinaryImage_DBSCAN::labels_)
        .def(nb::init<float, int>(), nb::arg("eps") = 1.5, nb::arg("min_samples") = 0)
        .def("fit", &libksc::clustering::BinaryImage_DBSCAN::fit, nb::arg("X"), nb::arg("y") = nb::none())
        .def("fit_predict", &libksc::clustering::BinaryImage_DBSCAN::fit_predict, nb::arg("X"), nb::arg("y") = nb::none())
    ;
}
