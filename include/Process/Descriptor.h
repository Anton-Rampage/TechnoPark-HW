#ifndef INCLUDE_PROCESS_DESCRIPTOR_H_
#define INCLUDE_PROCESS_DESCRIPTOR_H_

namespace process {
class Descriptor {
 public:
    explicit Descriptor(int fd);
    Descriptor();
    ~Descriptor() noexcept;
    Descriptor(Descriptor&) = delete;
    Descriptor(Descriptor&&);
    Descriptor& operator=(int fd);
    void close();
    int get() const;
    int extract();
    bool operator<(int num);
 private:
    int _fd = -1;
};
}  // namespace process

#endif  // INCLUDE_PROCESS_DESCRIPTOR_H_
