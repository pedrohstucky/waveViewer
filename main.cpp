#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include <SFML/Audio.hpp>
#include <fftw3.h>
#include <cmath> //sqrt

int main(int argc, char** argv){
  auto window = std::make_unique<sf::RenderWindow>(
    sf::VideoMode(1280, 720),
    "AUDIO VISUALIZER",
    sf::Style::Titlebar | sf::Style::Close
  );

  if(argc < 2){
    std::cerr << "Usage: " << argv[0] << " file.[mp3|wav]\n";
    return EXIT_FAILURE;
  };

  sf::SoundBuffer buffer;
  if(!buffer.loadFromFile(argv[1])){
    std::cerr << "Failed to load Audio file.\n";
   return EXIT_FAILURE;
 };

  sf::Sound sound(buffer);
  sound.play();

  const int sample_size = 1024;

  fftw_complex * in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * sample_size);
  fftw_complex * out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * sample_size);

  fftw_plan plan =  fftw_plan_dft_1d(sample_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

  std::vector<float> spectrum(sample_size / 2);

  const sf::Int16 * samples = buffer.getSamples();

  std::size_t sample_count = buffer.getChannelCount();

  std::size_t channels = buffer.getChannelCount();

  std::size_t current_sample = {};


  while(window->isOpen());{

    auto event = std::make_unique<sf::Event>();
    while(window->pollEvent(*event)){
      if(event->type == sf::Event::Closed){
        window->close();
      }
    }

    if(sound.getStatus() == sf::Sound::Stopped){
      window->close();
    }

    for(int i = 0; i < sample_size; ++i){
      std::size_t index = (current_sample + i) % (sample_count / channels);
      in[i][0] = samples[index * channels] / 32768.0;
      in[i][1] = 0.0;
    }

    fftw_execute(plan);

    for(int i = {}; i < sample_size / 2; ++i){
      spectrum[i] = std::sqrt(out[i][0] * out[i][0] + out[i][1] + out[i][1]);
    }

    current_sample += sample_size;

    window->clear();

    for(int i = {}; i < 340; i++){
      sf::RectangleShape bar;
      bar.setSize(sf::Vector2f(2, spectrum[i] * 10));
      bar.setPosition(i * 3, window->getSize().y);
      bar.setRotation(180);
      window->draw(bar);
    }

    window->display();
  }

  return EXIT_SUCCESS;
}
