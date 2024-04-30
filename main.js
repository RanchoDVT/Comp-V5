
function processFile() {
	const fileInput = document.getElementById('fileInput');
	const file = fileInput.files[0];

	// Perform file processing here
	const reader = new FileReader();
	reader.onload = function (e) {
		const fileContent = e.target.result;
		console.log('File content:', fileContent);
		// Your file processing code goes here
	};
	reader.readAsText(file);
}

async function processFile() {
	const fileInput = document.getElementById('fileInput');
	const file = fileInput.files[0];

	// Request permission to edit the file
	const permission = await navigator.permissions.query({ name: 'file-system' });
	if (permission.state === 'granted') {
		// Perform file processing here
		console.log('File editing permission granted');
		// Your file processing code goes here
	} else {
		console.log('File editing permission denied');
	}
}