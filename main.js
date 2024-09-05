marked.use({ breaks: true })
async function fetchReadme() {
  try {
    const response = await fetch('https://raw.githubusercontent.com/RanchoDVT/Comp-V5/dev/README.md');
    if (!response.ok) throw new Error('README not found');
    const text = await response.text();
    document.getElementById('readme-content').innerHTML = marked.parse(text);
  } catch (error) {
    console.error('Error fetching README:', error);
  }
}

async function fetchChangelog() {
  try {
    const response = await fetch('https://raw.githubusercontent.com/RanchoDVT/Comp-V5/dev/changelog.md');
    if (!response.ok) throw new Error('Changelog not found');
    const text = await response.text();
    document.getElementById('changelog-content').innerHTML = marked.parse(text);
  } catch (error) {
    console.error('Error fetching changelog:', error);
  }
}

document.addEventListener('DOMContentLoaded', () => {
  if (document.getElementById('readme-content')) {
    fetchReadme();
  }
  if (document.getElementById('changelog-content')) {
    fetchChangelog();
  }
});

// I hate js, why do I need a DOM loader check?
document.addEventListener('DOMContentLoaded', function () {
  var configForm = document.getElementById('config-form');
  var copyButton = document.getElementById('copy-button');
  var configOutput = document.getElementById('config-output');

  if (configForm) {
    configForm.addEventListener('submit', function (event) {
      event.preventDefault();
      const formData = new FormData(event.target);

      const config = `MOTOR_CONFIG
{
  FRONT_LEFT_MOTOR
  {
      PORT=${formData.get('front_left_port')}
      GEAR_RATIO=${formData.get('front_left_gear_ratio')}
      REVERSED=${formData.has('front_left_reversed')}
  }
  FRONT_RIGHT_MOTOR
  {
      PORT=${formData.get('front_right_port')}
      GEAR_RATIO=${formData.get('front_right_gear_ratio')}
      REVERSED=${formData.has('front_right_reversed')}
  }
  REAR_LEFT_MOTOR
  {
      PORT=${formData.get('rear_left_port')}
      GEAR_RATIO=${formData.get('rear_left_gear_ratio')}
      REVERSED=${formData.has('rear_left_reversed')}
  }
  REAR_RIGHT_MOTOR
  {
      PORT=${formData.get('rear_right_port')}
      GEAR_RATIO=${formData.get('rear_right_gear_ratio')}
      REVERSED=${formData.has('rear_right_reversed')}
  }
  INERTIAL
  {
      PORT=${formData.get('inertial_port')}
  }
  PRINTLOGO=${formData.has('print_logo')}
  LOGTOFILE=${formData.has('log_to_file')}
  MAXOPTIONSSIZE=${formData.get('max_options_size')}
  POLLINGRATE=${formData.get('polling_rate')}
  CTRLR1POLLINGRATE=${formData.get('ctrlr1_polling_rate')}
  VERSION=${formData.get('version')}
}`;

      configOutput.textContent = config;

      // Show the copy button once the config is generated
      copyButton.style.display = 'inline-block';
    });
  }

  if (copyButton) {
    copyButton.addEventListener('click', function () {
      if (configOutput.textContent) {
        navigator.clipboard.writeText(configOutput.textContent)
          .then(() => {
            console.debug('Config copied to clipboard!');
            const button =
              document.querySelector('copyButton');
              copyButton.innerHTML = 'Copied! ✅';
          })
          .catch(err => {
            console.error('Error copying text (Thats one rare error!): ', err);
          });
      }
    });
  }
});
